#ifndef _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__
#define _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__
#include <google/protobuf/service.h>
#include <pbrpc/error.h>
#include <pbrpc/types.h>

namespace pbrpc {
namespace client {

class SimpleTcpRpcController : public ::google::protobuf::RpcController {

public:
    
    SimpleTcpRpcController() : failed_(false) {}

    void Reset() {}

    bool Failed() const { return failed_; }

    std::string ErrorText() const { return errorText_; }

    void StartCancel() {}

    void SetFailed(const std::string& reason) { failed_ = true; errorText_ = reason; }

    bool IsCanceled() const { return false; }

    void NotifyOnCancel(::google::protobuf::Closure* callback) {}

private:

    bool failed_;
    std::string errorText_;
};

class SimpleTcpChannelImpl;
class SimpleTcpChannel : public ::google::protobuf::RpcChannel {
public:
    SimpleTcpChannel();
    ~SimpleTcpChannel();

    Error Init(uint32_t ip, uint16_t port);

    bool Bad();

    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
        ::google::protobuf::RpcController* controller,
        const ::google::protobuf::Message* request,
        ::google::protobuf::Message* response,
        ::google::protobuf::Closure* done);
private:
    SimpleTcpChannelImpl* impl_;
};

}
}

#endif // _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__