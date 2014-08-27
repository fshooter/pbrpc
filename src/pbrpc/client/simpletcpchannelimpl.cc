#include <WinSock2.h>
#include <pbrpc/proto/message.pb.h>
#include "simpletcpchannelimpl.h"

namespace pbrpc {
namespace client {

using namespace ::pbrpc::proto;

SimpleTcpChannelImpl::SimpleTcpChannelImpl() : initialized_(false)
{
    
}

SimpleTcpChannelImpl::~SimpleTcpChannelImpl()
{
    if (!initialized_)
        return;
    WSACleanup();
    closesocket(sock_);
}

Error SimpleTcpChannelImpl::Init(uint32_t ip, uint16_t port)
{
    idCounter_ = 0;

    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
        return Error::E_WIN32_ERR;

    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ == INVALID_SOCKET) {
        WSACleanup();
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

bool SimpleTcpChannelImpl::Bad() 
{
    if (!initialized_)
        return true;
    if (send(sock_, NULL, 0, 0) != 0)
        return true;
    return false;
}

void SimpleTcpChannelImpl::CallMethod(const MethodDescriptor* method,
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
    requestMsg.mutable_normal_request()->set_service_name(method->service()->full_name());
    requestMsg.mutable_normal_request()->set_method_name(method->name());
    requestMsg.mutable_normal_request()->set_request_body(request->SerializeAsString());

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
        if (!responseMsg.has_success_response()) {
            controller->SetFailed("Channel Error");
            done->Run();
            return;
        }
        if (!response->ParseFromString(responseMsg.success_response().response_body())) {
            controller->SetFailed("Channel Error");
            done->Run();
            return;
        }
        done->Run();
    }
    else if (responseMsg.type() == ResponseMsgType::FAIL) {
        if (!responseMsg.has_error_response()) {
            controller->SetFailed("Channel Error");
            done->Run();
            return;
        }
        controller->SetFailed(responseMsg.error_response().error_text());
        done->Run();
    }
    else {
        ChannelError();
        controller->SetFailed("Channel Error");
        done->Run();
        return;
    }
}

void SimpleTcpChannelImpl::ChannelError()
{
    initialized_ = false;
    closesocket(sock_);
}

}
}