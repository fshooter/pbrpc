#include <WinSock2.h>
#include <pbrpc/config.h>
#include <pbrpc/proto/message.pb.h>
#include "simpletcpchannelimpl.h"

namespace pbrpc {
namespace client {

using namespace ::pbrpc::proto;

bool SimpleTcpChannel::Impl::isWsaStartupCalled_ = false;

SimpleTcpChannel::Impl::Impl() : initialized_(false)
{
    
}

SimpleTcpChannel::Impl::~Impl()
{
    if (!initialized_)
        return;
    WSACleanup();
    closesocket(sock_);
}

Error SimpleTcpChannel::Impl::Init(uint32_t ip, uint16_t port)
{
    idCounter_ = 0;

    if (!isWsaStartupCalled_) {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(wVersionRequested, &wsaData) != 0)
            return Error::E_WIN32_ERR;
        isWsaStartupCalled_ = true;
    }

    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ == INVALID_SOCKET) {
        return Error::E_WIN32_ERR;
    }

    sockaddr_in endpoint = { 0 };
    endpoint.sin_family = AF_INET;
    endpoint.sin_addr.S_un.S_addr = ip;
    endpoint.sin_port = htons(port);

    if (connect(sock_, (sockaddr*)&endpoint, sizeof(endpoint)) != 0) {
        closesocket(sock_);
        WSACleanup();
        return Error::E_WIN32_ERR;
    }

    initialized_ = true;
    return Error::E_OK;
}

bool SimpleTcpChannel::Impl::Bad()
{
    if (!initialized_)
        return true;
    if (send(sock_, NULL, 0, 0) != 0)
        return true;
    return false;
}

void SimpleTcpChannel::Impl::CallMethod(const MethodDescriptor* method,
    RpcController* controller,
    const Message* request,
    Message* response,
    Closure* done)
{
    if (!initialized_) {
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }

    RequestMsg requestMsg;
    requestMsg.set_id(idCounter_++);
    requestMsg.set_type(RequestMsgType::NORMAL);
    requestMsg.set_service_name(method->service()->name());
    requestMsg.set_method_name(method->name());
    requestMsg.set_request_body(request->SerializeAsString());

    std::string requestBytes = requestMsg.SerializeAsString();
    uint32_t frameSize = requestBytes.size();

    if (send(sock_, (char*)&frameSize, sizeof(frameSize), 0) != sizeof(frameSize)) {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }
    
    if (send(sock_, requestBytes.c_str(), requestBytes.size(), 0) != requestBytes.size()) {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }

    if (recv(sock_, (char*)&frameSize, sizeof(frameSize), MSG_WAITALL) != sizeof(frameSize)
        || frameSize == 0) {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }

    std::vector<uint8_t> recvBytes;
    recvBytes.resize(frameSize);

    if (recv(sock_, (char*)&recvBytes[0], recvBytes.size(), MSG_WAITALL) != recvBytes.size()) {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }

    ResponseMsg responseMsg;
    if (!responseMsg.ParseFromArray(&recvBytes[0], recvBytes.size())) {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }

    if (responseMsg.id() != requestMsg.id()) {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }

    if (responseMsg.type() == ResponseMsgType::SUCCESS) {
        if (!responseMsg.has_response_body()) {
            controller->SetFailed("Channel Error");
            done->Run();
            return;
        }
        if (!response->ParseFromString(responseMsg.response_body())) {
            controller->SetFailed("Channel Error");
            done->Run();
            return;
        }
        done->Run();
    }
    else if (responseMsg.type() == ResponseMsgType::FAIL) {
        if (!responseMsg.has_error_text()) {
            controller->SetFailed("Channel Error");
            done->Run();
            return;
        }
        controller->SetFailed(responseMsg.error_text());
        done->Run();
    }
    else {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }
}

void SimpleTcpChannel::Impl::ChannelError()
{
    initialized_ = false;
    closesocket(sock_);
}



SimpleTcpChannelPool::Impl::Impl() : initialized_(false)
{

}

SimpleTcpChannelPool::Impl::~Impl()
{
    if (!initialized_)
        return;

    assert(busyChannels_.empty());

    for (ChannelContextVector::iterator it = freeChannels_.begin();
        it != freeChannels_.end();
        ++it)
    {
        delete (*it)->channel_;
        delete *it;
    }
    freeChannels_.clear();

    DeleteCriticalSection(&lock_);
}

Error SimpleTcpChannelPool::Impl::Init(uint32_t ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;
    minChannelNum_ = 5;
    maxChannelNum_ = 10;
    channelTimeout_ = 1000 * 10;
    return Error::E_OK;
}

void SimpleTcpChannelPool::Impl::SetMinChannelNum(int minChannelNum)
{
    minChannelNum_ = minChannelNum;
}

void SimpleTcpChannelPool::Impl::SetMaxChannelNum(int maxChannelNum)
{
    maxChannelNum_ = maxChannelNum;
}

void SimpleTcpChannelPool::Impl::SetChannelTimeout(unsigned channelTimeout)
{
    channelTimeout_ = channelTimeout;
}

SimpleTcpChannel* SimpleTcpChannelPool::Impl::GetChannel()
{
    DWORD currentTime = timeGetTime();
    
    EnterCriticalSection(&lock_);

    // 关闭所有空闲太久的Channel
redo:
    for (ChannelContextVector::iterator it = freeChannels_.begin();
        it != freeChannels_.end();
        ++it)
    {
        if (currentTime - (*it)->lastReleaseTime_ > (DWORD)channelTimeout_) {
            delete (*it)->channel_;
            delete *it;
            freeChannels_.erase(it);
            goto redo;
        }
    }

    ChannelContext* targetContext = NULL;
    while (!freeChannels_.empty()) {
        ChannelContextVector::iterator it = freeChannels_.begin();
        if ((*it)->channel_->Bad()) {
            delete (*it)->channel_;
            delete *it;
            freeChannels_.erase(it);
            continue;
        }
        targetContext = *it;
        freeChannels_.erase(it);
        break;
    }

    if (!targetContext) {
        // 需要新建一个连接, 在这之前,首先要确保连接数不能超限,如果超限,要阻塞等待
        while (busyChannels_.size() >= (size_t)maxChannelNum_) {
            LeaveCriticalSection(&lock_);
            ::Sleep(100);
            EnterCriticalSection(&lock_);
        }
        // 这儿开始实际的创建新连接了,创建连接可能比较慢,所以先释放锁,成功后再锁上
        LeaveCriticalSection(&lock_);
        SimpleTcpChannel* targetChannel = new SimpleTcpChannel;
        if (targetChannel->Init(ip_, port_) != Error::E_OK) {
            delete targetChannel;
            return NULL;
        }
        targetContext = new ChannelContext;
        targetContext->channel_ = targetChannel;
        targetContext->lastReleaseTime_ = 0;
        EnterCriticalSection(&lock_);
    }

    busyChannels_.push_back(targetContext);

    LeaveCriticalSection(&lock_);

    return targetContext->channel_;
}

Error SimpleTcpChannelPool::Impl::ReleaseChannel(SimpleTcpChannel* channel)
{
    EnterCriticalSection(&lock_);

    DWORD currentTime = timeGetTime();

    // 关闭所有空闲太久的Channel
redo:
    for (ChannelContextVector::iterator it = freeChannels_.begin();
        it != freeChannels_.end();
        ++it)
    {
        if (currentTime - (*it)->lastReleaseTime_ > (DWORD)channelTimeout_) {
            delete (*it)->channel_;
            delete *it;
            freeChannels_.erase(it);
            goto redo;
        }
    }

    // 找到Channel的上下文并从Busy列表中移除
    ChannelContext* targetContext = NULL;
    for (ChannelContextVector::iterator it = busyChannels_.begin();
        it != busyChannels_.end();
        ++it)
    {
        if ((*it)->channel_ == channel) {
            targetContext = *it;
            busyChannels_.erase(it);
            break;
        }
    }

    // 如果当前连接太多了或者目标Channel已经出错了,直接关闭Channel返回
    if (busyChannels_.size() + freeChannels_.size() >= (size_t)minChannelNum_
        || targetContext->channel_->Bad())
    {
        delete targetContext->channel_;
        delete targetContext;
        LeaveCriticalSection(&lock_);
        return Error::E_OK;
    }

    // 当连接放入空闲列表
    targetContext->lastReleaseTime_ = currentTime;
    freeChannels_.push_back(targetContext);
    
    LeaveCriticalSection(&lock_);
    return Error::E_OK;
}

}
}