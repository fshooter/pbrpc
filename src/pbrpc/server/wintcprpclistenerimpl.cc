#include <WinSock2.h>
#include <mswsock.h>
#include <google/protobuf/stubs/common.h>
#include <pbrpc/common/log.h>
#include "wintcprpclistenerimpl.h"

#define MAX_PARALLEL_REQUESTS_PER_SOCKET 100
#define MAX_OUTSTANDING_METHOD_CALLS 10000
#define MAX_PACKET_SIZE 1024 * 1024

namespace pbrpc {
namespace server {

using namespace ::pbrpc::common;

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

    if (status_ == Status::PAUSED) {
        WSACleanup();
    }
}

Error WinTcpRpcListenerImpl::Init(uint32_t ip, uint16_t port)
{
    if (status_ != Status::UNINITIALIZED)
        return Error::E_INVALID_STATUS;

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

    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
        return Error::E_WIN32_ERR;

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

    completionPort_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
    if (completionPort_ == NULL) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }
    
    sock_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock_ == INVALID_SOCKET) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

    if (!CreateIoCompletionPort((HANDLE)sock_, completionPort_, NULL, 1)) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

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

    thread_ = CreateThread(NULL, 0,
        &WinTcpRpcListenerImpl::WorkerThreadRoutineStub, this,
        0, NULL);
    if (!thread_) {
        err = Error::E_WIN32_ERR;
        goto cleanup;
    }

    status_ = Status::RUNNING;

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
    
    status_ = Status::STOPPING_1;

    PerIoDataStopping* stoppingData = new PerIoDataStopping;
    memset(stoppingData, 0, sizeof(PerIoDataStopping));
    stoppingData->type = IoType::LISTENER_STOPPING;

    if (!PostQueuedCompletionStatus(completionPort_, 0, NULL, &stoppingData->overlapped))
        Fetal("Error\n");

    while (currentAcceptSock_ != INVALID_SOCKET)
        ::Sleep(100);

    while (!ListHeadEmpty(&outstandingMethodCalls_))
        ::Sleep(100);

    status_ = Status::STOPPING_2;

    WaitForSingleObject(thread_, INFINITE);
    CloseHandle(thread_);
    thread_ = NULL;

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
    case ERROR_IO_PENDING:
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

    Status statusSnap = status_;

    SOCKET sock = acceptData->sock;

    if (!success) {
        goto cleanup;
    }

    if (statusSnap == Status::STOPPING_1) {
        success = false;
        goto cleanup;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&sock_, sizeof(sock_)) != 0) {
        success = false;
        goto cleanup;
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
        success = false;
        goto cleanup;
    }
    if (WSAAddressToString(remoteAddr, remoteAddrLen, NULL, remoteAddrStr, &remoteAddrStrLen) != 0) {
        success = false;
        goto cleanup;
    }
    TraceInfo("Accept, LocalAddr=%s, RemoteAddr=%s\n", localAddrStr, remoteAddrStr);

    SocketContext* socketContext = new SocketContext;
    socketContext->sock = sock;
    ListHeadInitailize(&socketContext->outstandingMethodCalls);

    DoRecv(socketContext);

cleanup:

    delete acceptData;

    if (!success) {
        closesocket(sock);
    }

    if (statusSnap != Status::STOPPING_1)
        DoAccept();
    else
        currentAcceptSock_ = INVALID_SOCKET;
}

void WinTcpRpcListenerImpl::DoRecv(SocketContext* socketContext)
{
    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);

    Omc* omc = new Omc;
    omc->status = OmcStatus::RECVING;
    omc->socketContext = socketContext;
    omc->specific.recving = new OmcRecving;
    omc->specific.recving->requestSize = 0;
    ListHeadInsertTail(&socketContext->outstandingMethodCalls,
        &omc->outstandingMethodCallsSocketEntry);
    ListHeadInsertTail(&outstandingMethodCalls_, &omc->outstandingMethodCallsEntry);

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

    WSABUF buf = { 0 };
    if (!omc->specific.recving->requestSize) {
        omc->specific.recving->request.resize(sizeof(uint32_t));
        buf.buf = (char*)omc->specific.recving->request.c_str();
        buf.len = sizeof(uint32_t);
    }
    else {
        if (omc->specific.recving->requestSize > MAX_PACKET_SIZE) {
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

    assert(status_ == Status::RUNNING || status_ == Status::STOPPING_1);
    assert(omc->status == OmcStatus::RECVING);

    if (!success)
        goto cleanup;

    if (status_ == Status::STOPPING_1) {
        success = false;
        goto cleanup;
    }

    DWORD bytesTransfer = 0;
    DWORD flags = 0;
    if (!WSAGetOverlappedResult(socketContext->sock, &recvData->overlapped,
        &bytesTransfer, FALSE, &flags)) {
        success = false;
        goto cleanup;
    }

    if (omc->specific.recving->requestSize == 0) {
        if (bytesTransfer != sizeof(uint32_t)) {
            success = false;
            goto cleanup;
        }
        omc->specific.recving->requestSize = *(uint32_t*)omc->specific.recving->request.c_str();
        DoRecv(omc);
    }
    else {
        if (bytesTransfer != omc->specific.recving->requestSize) {
            success = false;
            goto cleanup;
        }

        RequestMsg requestMsg;
        if (!requestMsg.ParseFromString(omc->specific.recving->request)) {
            success = false;
            goto cleanup;
        }

        if (requestMsg.type() == RequestMsgType::NORMAL) {
            const NormalRequestBody& normalRequest = requestMsg.normal_request();
            Service* service = methodCallback_->QueryService(normalRequest.service_name());
            if (!service) {
                success = false;
                goto cleanup;
            }
            const MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(normalRequest.method_name());
            if (!method) {
                success = false;
                goto cleanup;
            }
            Message* realRequest = service->GetRequestPrototype(method).New();
            assert(realRequest);
            if (!realRequest->ParseFromString(normalRequest.request_body())) {
                delete realRequest;
                success = false;
                goto cleanup;
            }
            Message* realResponse = service->GetResponsePrototype(method).New();
            assert(realResponse);

            delete omc->specific.recving;
            omc->status = OmcStatus::PROCESSING;
            omc->specific.processing = new OmcProcessing;
            omc->specific.processing->id = requestMsg.id();
            omc->specific.processing->request = realRequest;
            omc->specific.processing->response = realResponse;

            DoRecv(omc->socketContext);

            methodCallback_->OnMethodCall(service, method,
                &omc->specific.processing->controller,
                omc->specific.processing->request,
                omc->specific.processing->response,
                NewCallback(this, &WinTcpRpcListenerImpl::OnMethodCallDoneThreadSafe, omc));
        }
        else if (requestMsg.type() == RequestMsgType::CANCEL) {
            omc->specific.recving->requestSize = 0;
            omc->specific.recving->request.clear();
            DoRecv(omc);
        }
        else {
            success = false;
            goto cleanup;
        }
    }

cleanup:
    free(recvData);
    if (!success) {
        shutdown(socketContext->sock, SD_BOTH);
        CloseOmc(omc);
    }
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
        TraceWarning("OnSendFinish! Send Failed\n");
        goto err;
    }

    DWORD bytesTransfer = 0;
    DWORD flags = 0;
    if (!WSAGetOverlappedResult(socketContext->sock, &sendData->overlapped,
        &bytesTransfer, FALSE, &flags)) {
        goto err;
    }

    if (!omcSending->frameHeaderSended && bytesTransfer < sizeof(uint32_t)) {
        goto err;
    }

    if (!omcSending->frameHeaderSended) {
        omcSending->frameHeaderSended = true;
        omcSending->bytesLeftToSend -= (bytesTransfer - sizeof(uint32_t));
    }
    else {
        omcSending->bytesLeftToSend -= bytesTransfer;
    }

    if (omcSending->bytesLeftToSend != 0) {
        delete sendData;
        return DoSend(omc);
    }

    delete sendData;
    CloseOmc(omc);
    return;

err:

    delete sendData;
    shutdown(socketContext->sock, SD_BOTH);
    CloseOmc(omc);
}

void WinTcpRpcListenerImpl::CloseOmc(Omc* omc)
{
    ListHeadRemove(&omc->outstandingMethodCallsEntry);
    ListHeadRemove(&omc->outstandingMethodCallsSocketEntry);

    if (ListHeadEmpty(&omc->socketContext->outstandingMethodCalls)) {
        closesocket(omc->socketContext->sock);
        delete omc->socketContext;
    }

    switch (omc->status) {
    case OmcStatus::RECVING:
        delete omc->specific.recving;
        break;
    case OmcStatus::PROCESSING:
        if (omc->specific.processing->request)
            delete omc->specific.processing->request;
        if (omc->specific.processing->response)
            delete omc->specific.processing->response;
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
        responseMsg.mutable_error_response()->set_error_text(
            omcProcessing->controller.ErrorText());
    }
    else {
        std::string responseBytes;
        if (!omcProcessing->response->SerializePartialToString(&responseBytes)) {
            responseMsg.set_id(omcProcessing->id);
            responseMsg.set_type(ResponseMsgType::FAIL);
            responseMsg.mutable_error_response()->set_error_text(
                "Response Serial Error");
        }
        else {
            responseMsg.set_id(omcProcessing->id);
            responseMsg.set_type(ResponseMsgType::SUCCESS);
            responseMsg.mutable_success_response()->set_response_body(
                responseBytes);
        }
    }

    delete omcProcessing->request;
    delete omcProcessing->response;

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