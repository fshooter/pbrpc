/*!
 * @file wintcprpclistenerimpl.cc
 * @brief WinTcpRpcListenerImpl类实现文件
 */
#include <WinSock2.h>
#include <mswsock.h>
#include <google/protobuf/stubs/common.h>
#include <pbrpc/config.h>
#include <pbrpc/common/log.h>
#include "wintcprpclistenerimpl.h"

#define MAX_OUTSTANDING_METHOD_CALLS 10000 ///< 最多同时进行的Rpc调用数量
#define MAX_PACKET_SIZE 1024 * 1024 ///< 每个帧的最大大小

namespace pbrpc {
namespace server {

using namespace ::pbrpc::common;

bool WinTcpRpcListenerImpl::isWsaStartupCalled_ = false;

WinTcpRpcListenerImpl::WinTcpRpcListenerImpl() : status_(Status::UNINITIALIZED)
{
}

WinTcpRpcListenerImpl::~WinTcpRpcListenerImpl()
{
    assert(status_ == Status::UNINITIALIZED ||
        status_ == Status::PAUSED ||
        status_ == Status::RUNNING);

    if (status_ == Status::RUNNING) {
        if (Stop() != Error::E_OK)
            Fetal("Stop Error");
    }
}

Error WinTcpRpcListenerImpl::Init(uint32_t ip, uint16_t port)
{
    if (status_ != Status::UNINITIALIZED)
        return Error::E_INVALID_STATUS;

    if (!isWsaStartupCalled_) {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(wVersionRequested, &wsaData) != 0)
            return Error::E_WIN32_ERR;
        isWsaStartupCalled_ = true;
    }

    ip_ = ip;
    port_ = port;
    methodCallback_ = NULL;
    sock_ = INVALID_SOCKET;
    completionPort_ = NULL;
    thread_ = NULL;
    acceptExRoutine_ = NULL;
    getAcceptExSockaddrsRoutine_ = NULL;
    currentAcceptSock_ = INVALID_SOCKET;
    ListHeadInitailize(&outstandingMethodCalls_);
    outstandingMethodCallsNum_ = 0;

    status_ = Status::PAUSED;

    return Error::E_OK;
}

Error WinTcpRpcListenerImpl::GetMethodCallback(MethodCallback** cb) const
{
    if (status_ != Status::PAUSED)
        return Error::E_INVALID_STATUS;
    *cb = methodCallback_;
    return Error::E_OK;
}

Error WinTcpRpcListenerImpl::SetMethodCallback(MethodCallback* callback)
{
    if (status_ != Status::PAUSED)
        return Error::E_INVALID_STATUS;
    methodCallback_ = callback;
    return Error::E_OK;
}

Error WinTcpRpcListenerImpl::Start()
{
    if (status_ != Status::PAUSED)
        return Error::E_INVALID_STATUS;
    if (!methodCallback_)
        return Error::E_INVALID_PARAMETER;

    Error err = Error::E_OK;

    // 创建完成端口
    completionPort_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
    if (completionPort_ == NULL) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }
    
    // 创建监听SOCKET
    sock_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock_ == INVALID_SOCKET) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

    // 将SOCKET绑定到完成端口
    if (!CreateIoCompletionPort((HANDLE)sock_, completionPort_, NULL, 1)) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

    // 绑定并开始监听
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.S_un.S_addr = ip_;
    sa.sin_port = htons(port_);
    if (::bind(sock_, (sockaddr*)&sa, sizeof(sa)) != 0 ||
        ::listen(sock_, SOMAXCONN) != 0) 
    {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

    // 取得必要的函数地址
    if (!acceptExRoutine_) {
        GUID guidAcceptEx = WSAID_ACCEPTEX;
        DWORD bytesRet = 0;
        if (WSAIoctl(sock_, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guidAcceptEx, sizeof(guidAcceptEx),
            &acceptExRoutine_, sizeof(acceptExRoutine_),
            &bytesRet, NULL, NULL) != 0) {
            err = Error::E_WIN32_ERR;
            goto cleanup;
        }
    }

    if (!getAcceptExSockaddrsRoutine_) {
        GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
        DWORD bytesRet = 0;
        if (WSAIoctl(sock_, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
            &getAcceptExSockaddrsRoutine_, sizeof(getAcceptExSockaddrsRoutine_),
            &bytesRet, NULL, NULL) != 0) {
            err = Error::E_WIN32_ERR;
            goto cleanup;
        }
    }

    // 创建IO线程
    thread_ = CreateThread(NULL, 0,
        &WinTcpRpcListenerImpl::WorkerThreadRoutineStub, this,
        0, NULL);
    if (!thread_) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

    // 初始化成功了,切换状态
    status_ = Status::RUNNING;

    // 开始异步Accept
    DoAccept();

cleanup:

    if (err != Error::E_OK) {

        if (sock_ != INVALID_SOCKET) {
            closesocket(sock_);
            sock_ = INVALID_SOCKET;
        }

        if (!completionPort_) {
            CloseHandle(completionPort_);
            completionPort_ = NULL;
        }
    }

    return err;
}

Error WinTcpRpcListenerImpl::Stop()
{
    if (status_ != Status::RUNNING)
        return Error::E_INVALID_STATUS;
    
    //
    // 首先切换状态到 STOPPING_1 并发送退出消息到IO线程
    // 这样IO线程就不会接受新的Rpc请求了
    // 然后我们一直等待直到所有已建立的Rpc请求完成
    //

    status_ = Status::STOPPING_1;

    PerIoDataStopping* stoppingData = new PerIoDataStopping;
    memset(stoppingData, 0, sizeof(PerIoDataStopping));
    stoppingData->type = IoType::LISTENER_STOPPING;

    if (!PostQueuedCompletionStatus(completionPort_, 0, NULL, &stoppingData->overlapped))
        Fetal("PostQueuedCompletionStatus Error\n");

    while (currentAcceptSock_ != INVALID_SOCKET)
        ::Sleep(100);

    while (!ListHeadEmpty(&outstandingMethodCalls_))
        ::Sleep(100);

    //
    // 这时候可以切换到 STOPPING_2 了
    // IO线程在发现此状态时会退出
    // 我们等待IO线程退出
    //

    status_ = Status::STOPPING_2;

    WaitForSingleObject(thread_, INFINITE);
    CloseHandle(thread_);
    thread_ = NULL;

    //
    // 这时候可以释放其他的资源了
    //

    CloseHandle(completionPort_);
    completionPort_ = NULL;

    closesocket(sock_);
    sock_ = INVALID_SOCKET;

    status_ = Status::PAUSED;

    return Error::E_OK;
}

void WinTcpRpcListenerImpl::DoAccept()
{
    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);

    SOCKET acceptSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (acceptSock == INVALID_SOCKET) {
        Fetal("Fetal Error In DoAccept\n");
    }

    PerIoDataAccept* acceptData = new PerIoDataAccept;
    assert(acceptData);
    memset(acceptData, 0, sizeof(*acceptData));

    acceptData->type = IoType::ACCEPT;
    acceptData->sock = acceptSock;

    currentAcceptSock_ = acceptSock;

    DWORD bytesRet = 0;
    if (acceptExRoutine_(sock_, acceptSock,
        acceptData->recvBuffer, 0,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
        &bytesRet, &acceptData->overlapped))
    {
        OnAcceptFinish(true, acceptData);
        return;
    }

    switch (WSAGetLastError()) {
    case WSA_IO_PENDING:
        if (CreateIoCompletionPort((HANDLE)acceptSock, completionPort_, NULL, 0) == NULL) {
            Fetal("Fetal Error In DoAccept\n");
        }
        break;
    case WSAECONNRESET:
        OnAcceptFinish(false, acceptData);
        break;
    default:
        Fetal("Fetal Error In DoAccept\n");
    }
}

void WinTcpRpcListenerImpl::OnAcceptFinish(bool success, PerIoDataAccept* acceptData)
{
    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);

    SOCKET sock = acceptData->sock;

    if (status_ == Status::STOPPING_1) {
        delete acceptData;
        closesocket(sock);
        currentAcceptSock_ = INVALID_SOCKET;
        return;
    }

    if (!success) {
        goto err;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&sock_, sizeof(sock_)) != 0) {
        goto err;
    }

    sockaddr *localAddr, *remoteAddr;
    int localAddrLen, remoteAddrLen;
    getAcceptExSockaddrsRoutine_(acceptData->recvBuffer, 0,
        sizeof(sockaddr) + 16, sizeof(sockaddr) + 16,
        &localAddr, &localAddrLen, &remoteAddr, &remoteAddrLen);

    char localAddrStr[32];
    char remoteAddrStr[32];
    DWORD localAddrStrLen = sizeof(localAddrStr);
    DWORD remoteAddrStrLen = sizeof(remoteAddrStr);

    if (WSAAddressToString(localAddr, localAddrLen, NULL, localAddrStr, &localAddrStrLen) != 0) {
        goto err;
    }
    if (WSAAddressToString(remoteAddr, remoteAddrLen, NULL, remoteAddrStr, &remoteAddrStrLen) != 0) {
        goto err;
    }
    TraceInfo("Accept, LocalAddr=%s, RemoteAddr=%s\n", localAddrStr, remoteAddrStr);

    delete acceptData;
    acceptData = NULL;

    // accept成功了,建立 SocketContext
    SocketContext* socketContext = new SocketContext;
    socketContext->sock = sock;
    ListHeadInitailize(&socketContext->outstandingMethodCalls);

    DoRecv(socketContext);
    DoAccept();
    return;

err:
    if (acceptData) {
        delete acceptData;
    }
    closesocket(sock);
    DoAccept();
}

void WinTcpRpcListenerImpl::DoRecv(SocketContext* socketContext)
{
    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);

    /* 对于一个空闲的 SocketContext, 创建一个 RECVING 状态的 Omc */
    Omc* omc = new Omc;
    omc->status = OmcStatus::RECVING;
    omc->socketContext = socketContext;
    omc->specific.recving = new OmcRecving;
    omc->specific.recving->requestSize = 0;
    ListHeadInsertTail(&socketContext->outstandingMethodCalls,
        &omc->outstandingMethodCallsSocketEntry);
    ListHeadInsertTail(&outstandingMethodCalls_, &omc->outstandingMethodCallsEntry);
    outstandingMethodCallsNum_ += 1;

    DoRecv(omc);
}

void WinTcpRpcListenerImpl::DoRecv(Omc* omc)
{
    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);
    assert(omc->status == OmcStatus::RECVING);

    PerIoDataRecv* recvData = new PerIoDataRecv;
    assert(recvData);

    memset(recvData, 0, sizeof(PerIoDataRecv));
    recvData->type = IoType::RECV;
    recvData->omc = omc;

    // OmcRecving::requestSize == 0说明我们还没有接收到帧长度
    // 那么首先接收帧长度
    // 否则接收帧数据
    WSABUF buf = { 0 };
    if (!omc->specific.recving->requestSize) {
        // 接收帧长度
        omc->specific.recving->request.resize(sizeof(uint32_t));
        buf.buf = (char*)omc->specific.recving->request.c_str();
        buf.len = sizeof(uint32_t);
    }
    else {
        // 已经接收到了帧长度,检验长度是否合法
        if (omc->specific.recving->requestSize > MAX_PACKET_SIZE) {
            // 不合法直接关闭
            shutdown(omc->socketContext->sock, SD_BOTH);
            CloseOmc(omc);
            delete recvData;
            return;
        }
        omc->specific.recving->request.resize(omc->specific.recving->requestSize);
        buf.buf = (char*)omc->specific.recving->request.c_str();
        buf.len = omc->specific.recving->requestSize;
    }

    DWORD flags = MSG_WAITALL;
    int ret = WSARecv(omc->socketContext->sock, &buf, 1, NULL, &flags,
        &recvData->overlapped, NULL);

    if (ret == 0 || WSAGetLastError() == WSA_IO_PENDING) {
        return;
    }
    else {
        OnRecvFinish(false, recvData);
    }
}

void WinTcpRpcListenerImpl::OnRecvFinish(bool success, PerIoDataRecv* recvData)
{
    Omc* omc = recvData->omc;
    SocketContext* socketContext = omc->socketContext;

    int id;
    std::string reason;

    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);
    assert(omc->status == OmcStatus::RECVING);

    if (!success)
        goto closechannel;

    if (status_ == Status::STOPPING_1) {
        goto closechannel;
    }

    DWORD bytesTransfer = 0;
    DWORD flags = 0;
    if (!WSAGetOverlappedResult(socketContext->sock, &recvData->overlapped,
        &bytesTransfer, FALSE, &flags)) {
        goto closechannel;
    }

    delete recvData;
    recvData = NULL;

    if (omc->specific.recving->requestSize == 0) {
        // 我们接受的是帧长度
        if (bytesTransfer != sizeof(uint32_t)) {
            goto closechannel;
        }
        omc->specific.recving->requestSize = *(uint32_t*)omc->specific.recving->request.c_str();
        DoRecv(omc);
        return;
    }
    else {
        // 我们接受的是帧数据
        if (bytesTransfer != omc->specific.recving->requestSize) {
            goto closechannel;
        }

        // 解析请求消息
        RequestMsg requestMsg;
        if (!requestMsg.ParseFromString(omc->specific.recving->request)) {
            success = false;
            goto closechannel;
        }

        id = requestMsg.id();

        if (requestMsg.type() == RequestMsgType::NORMAL) {

            // 处理普通请求

            if (outstandingMethodCallsNum_ > MAX_OUTSTANDING_METHOD_CALLS) {
                reason = "Server Busy";
                goto sendfailresponse;
            }

            if (!requestMsg.has_service_name() ||
                !requestMsg.has_method_name() ||
                !requestMsg.has_request_body()) {
                reason = "Invalid Request";
                goto sendfailresponse;
            }

            Service* service = methodCallback_->QueryService(requestMsg.service_name());
            if (!service) {
                reason = "No Such Service";
                goto sendfailresponse;
            }
            const MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(requestMsg.method_name());
            if (!method) {
                reason = "No Such Method";
                goto sendfailresponse;
            }
            Message* realRequest = service->GetRequestPrototype(method).New();
            assert(realRequest);
            if (!realRequest->ParseFromString(requestMsg.request_body())) {
                delete realRequest;
                reason = "Invalid Request";
                goto sendfailresponse;
            }
            Message* realResponse = service->GetResponsePrototype(method).New();
            assert(realResponse);

            // 转换 Omc 到 PROCESSING 状态
            delete omc->specific.recving;
            omc->status = OmcStatus::PROCESSING;
            omc->specific.processing = new OmcProcessing;
            omc->specific.processing->id = requestMsg.id();
            omc->specific.processing->request = realRequest;
            omc->specific.processing->response = realResponse;

            // 开始接收下一个请求
            // DoRecv 不能在 OnMethodCall 之后调用
            // 因为OnMethodCall调用之后我们就交出了Omc的使用权
            DoRecv(omc->socketContext);

            // 执行回调
            methodCallback_->OnMethodCall(service, method,
                &omc->specific.processing->controller,
                omc->specific.processing->request,
                omc->specific.processing->response,
                NewCallback(this, &WinTcpRpcListenerImpl::OnMethodCallDoneThreadSafe, omc));

            return;
        }
        else if (requestMsg.type() == RequestMsgType::CANCEL) {

            // 处理取消请求

            // 依次遍历当前Socket上的所有 PROCESSING 状态的 Omc
            for (ListHead* entry = socketContext->outstandingMethodCalls.next;
                entry != &socketContext->outstandingMethodCalls;
                entry = entry->next)
            {
                Omc* pendingOmc = CONTAINING_RECORD(entry, Omc, outstandingMethodCallsSocketEntry);
                if (pendingOmc->status == OmcStatus::PROCESSING
                    && pendingOmc->specific.processing->id == id)
                {
                    pendingOmc->specific.processing->controller.StartCancel();
                    break;
                }
            }

            // 处理完取消请求之后,我们需要继续在此 Omc 上接收其他Rpc调用
            omc->specific.recving->requestSize = 0;
            omc->specific.recving->request.clear();
            DoRecv(omc);
            return;
        }
        else {

            // 非法请求

            goto closechannel;
        }
    }

closechannel:
    if (recvData) {
        delete recvData;
    }
    shutdown(socketContext->sock, SD_BOTH);
    CloseOmc(omc);
    return;

sendfailresponse:
    if (recvData) {
        delete recvData;
    }
    ResponseMsg response;
    response.set_id(id);
    response.set_type(ResponseMsgType::FAIL);
    response.set_error_text(reason);
    delete omc->specific.recving;
    omc->status = OmcStatus::SENDING;
    omc->specific.sending = new OmcSending;
    omc->specific.sending->frameHeaderSended = false;
    if (!response.SerializeToString(&omc->specific.sending->response))
        goto closechannel;
    omc->specific.sending->bytesLeftToSend = omc->specific.sending->response.size();
    DoSend(omc);
    return;
}

void WinTcpRpcListenerImpl::DoSend(Omc* omc)
{
    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);
    assert(omc->status == OmcStatus::SENDING);

    PerIoDataSend* sendData = new PerIoDataSend;
    assert(sendData);

    memset(sendData, 0, sizeof(PerIoDataSend));
    sendData->type = IoType::SEND;
    sendData->omc = omc;

    WSABUF buf[2];
    memset(buf, 0, sizeof(buf));
    int bufCount = 1;

    // 如果没有发送过帧长度,那么发送帧长度+帧数据.
    // 否则只发送帧数据, WSASend有可能只发送了部分数据,因此从上次遗留的部分接着发
    if (!omc->specific.sending->frameHeaderSended) {
        bufCount = 2;
        buf[0].buf = (char*)&omc->specific.sending->bytesLeftToSend;
        buf[0].len = sizeof(uint32_t);
        buf[1].buf = (char*)omc->specific.sending->response.c_str();
        buf[1].len = omc->specific.sending->bytesLeftToSend;
    }
    else {
        buf[0].buf = (char*)omc->specific.sending->response.c_str()
            + omc->specific.sending->response.size()
            - omc->specific.sending->bytesLeftToSend;
        buf[0].len = omc->specific.sending->bytesLeftToSend;
    }
    
    int ret = WSASend(omc->socketContext->sock, buf, bufCount, NULL, 0, &sendData->overlapped, NULL);

    if (ret == 0 || WSAGetLastError() == WSA_IO_PENDING) {
        return;
    }
    else {
        return OnSendFinish(false, sendData);
    }
}

void WinTcpRpcListenerImpl::OnSendFinish(bool success, PerIoDataSend* sendData)
{
    Omc* omc = sendData->omc;
    OmcSending* omcSending = omc->specific.sending;
    SocketContext* socketContext = omc->socketContext;

    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);
    assert(omc->status == OmcStatus::SENDING);

    if (!success) {
        goto err;
    }

    DWORD bytesTransfer = 0;
    DWORD flags = 0;
    if (!WSAGetOverlappedResult(socketContext->sock, &sendData->overlapped,
        &bytesTransfer, FALSE, &flags)) {
        goto err;
    }

    delete sendData;
    sendData = NULL;

    if (!omcSending->frameHeaderSended && bytesTransfer < sizeof(uint32_t)) {
        goto err;
    }

    // 计算还有多少数据没有发送
    if (!omcSending->frameHeaderSended) {
        omcSending->frameHeaderSended = true;
        omcSending->bytesLeftToSend -= (bytesTransfer - sizeof(uint32_t));
    }
    else {
        omcSending->bytesLeftToSend -= bytesTransfer;
    }

    if (omcSending->bytesLeftToSend != 0) {
        // 还没发送完,接着发
        return DoSend(omc);
    }

    // 发送完了,这次Rpc处理过程也就结束了,关闭 Omc
    CloseOmc(omc);
    return;

err:
    if (sendData) {
        delete sendData;
    }
    shutdown(socketContext->sock, SD_BOTH);
    CloseOmc(omc);
}

void WinTcpRpcListenerImpl::CloseOmc(Omc* omc)
{
    // 首先从活动列表中移除 Omc
    ListHeadRemove(&omc->outstandingMethodCallsEntry);
    outstandingMethodCallsNum_ -= 1;
    ListHeadRemove(&omc->outstandingMethodCallsSocketEntry);

    // 如果 SocketContext 上的所有 Omc 都完成了,删除SocketContext
    if (ListHeadEmpty(&omc->socketContext->outstandingMethodCalls)) {
        closesocket(omc->socketContext->sock);
        delete omc->socketContext;
    }

    // 根据 Omc 的状态释放内容
    switch (omc->status) {
    case OmcStatus::RECVING:
        delete omc->specific.recving;
        break;
    case OmcStatus::PROCESSING:
        if (omc->specific.processing->request)
            delete omc->specific.processing->request;
        if (omc->specific.processing->response)
            delete omc->specific.processing->response;
        delete omc->specific.processing;
        break;
    case OmcStatus::SENDING:
        delete omc->specific.sending;
        break;
    default:
        Fetal("Error CloseOmc\n");
    }

    delete omc;
}

void WinTcpRpcListenerImpl::OnMethodCallDone(PerIoDataMethodCallDone* doneData)
{
    Omc* omc = doneData->omc;

    assert(omc->status == OmcStatus::PROCESSING);

    delete doneData;

    SocketContext* sockCtx = omc->socketContext;

    OmcProcessing* omcProcessing = omc->specific.processing;

    ResponseMsg responseMsg;
    if (omcProcessing->controller.Failed()) {
        responseMsg.set_id(omcProcessing->id);
        responseMsg.set_type(ResponseMsgType::FAIL);
        responseMsg.set_error_text(
            omcProcessing->controller.ErrorText());
    }
    else {
        std::string responseBytes;
        if (!omcProcessing->response->SerializePartialToString(&responseBytes)) {
            responseMsg.set_id(omcProcessing->id);
            responseMsg.set_type(ResponseMsgType::FAIL);
            responseMsg.set_error_text(
                "Response Serial Error");
        }
        else {
            responseMsg.set_id(omcProcessing->id);
            responseMsg.set_type(ResponseMsgType::SUCCESS);
            responseMsg.set_response_body(
                responseBytes);
        }
    }

    delete omcProcessing->request;
    delete omcProcessing->response;
    delete omcProcessing;

    omc->status = OmcStatus::SENDING;
    omc->specific.sending = new OmcSending;
    omc->specific.sending->frameHeaderSended = false;
    if (!responseMsg.SerializePartialToString(&omc->specific.sending->response)) {
        Fetal("Error\n");
    }
    omc->specific.sending->bytesLeftToSend = omc->specific.sending->response.size();

    DoSend(omc);
}

void WinTcpRpcListenerImpl::OnMethodCallDoneThreadSafe(Omc* omc)
{
    PerIoDataMethodCallDone* doneData = new PerIoDataMethodCallDone;
    assert(doneData);
    memset(doneData, 0, sizeof(PerIoDataMethodCallDone));
    doneData->type = IoType::METHOD_CALL_DONE;
    doneData->omc = omc;
    if (!PostQueuedCompletionStatus(completionPort_, 1, NULL, &doneData->overlapped)) {
        Fetal("PostQueuedCompletionStatus Failed\n");
    }
}

void WinTcpRpcListenerImpl::OnDataStopping(PerIoDataStopping* stopData)
{
    delete stopData;

    if (currentAcceptSock_ != INVALID_SOCKET) {
        // 这儿强制使用了closesocket
        // 最好是有一种安全的方式能够打断AcceptEx而不用关闭句柄
        // 因为关闭句柄后可能该句柄值会被重用,然后OnAcceptFinish之后会再次关闭句柄可能就关错了
        // 目前没有更好的方式,先这样吧,概率还是比较小的
        closesocket(currentAcceptSock_);
    }
    
    // 关闭所有处于 RECVING 状态的omc
    for (ListHead* current = outstandingMethodCalls_.next;
        current != &outstandingMethodCalls_;
        current = current->next)
    {
        Omc* omc = CONTAINING_RECORD(current, Omc, outstandingMethodCallsEntry);
        if (omc->status == OmcStatus::RECVING)
            shutdown(omc->socketContext->sock, SD_BOTH);
    }
}

void WinTcpRpcListenerImpl::WorkerThreadRoutine()
{
    while (status_ != Status::STOPPING_2)
    {
        DWORD bytesCount = 0;
        ULONG_PTR completionKey = NULL;
        OVERLAPPED* overlapped = NULL;

        bool success = (GetQueuedCompletionStatus(completionPort_, &bytesCount,
            &completionKey, &overlapped, 1000) == TRUE);

        if (!overlapped)
            continue;

        PerIoData* ioData = CONTAINING_RECORD(overlapped, PerIoData, overlapped);
        switch (ioData->type) {
        case IoType::ACCEPT:
            OnAcceptFinish(success, (PerIoDataAccept*)ioData);
            break;
        case IoType::SEND:
            OnSendFinish(success, (PerIoDataSend*)ioData);
            break;
        case IoType::RECV:
            OnRecvFinish(success, (PerIoDataRecv*)ioData);
            break;
        case IoType::METHOD_CALL_DONE:
            OnMethodCallDone((PerIoDataMethodCallDone*)ioData);
            break;
        case IoType::LISTENER_STOPPING:
            OnDataStopping((PerIoDataStopping*)ioData);
            break;
        default:
            Fetal("Unknonw IoType\n");
        }
    }
}

DWORD WinTcpRpcListenerImpl::WorkerThreadRoutineStub(LPVOID self)
{
    ((WinTcpRpcListenerImpl*)(self))->WorkerThreadRoutine();
    return 0;
}

}
}