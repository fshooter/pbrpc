#ifndef _PBPRC_SERVER_RPC_LISTENER_H__
#define _PBPRC_SERVER_RPC_LISTENER_H__
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <pbrpc/error.h>

namespace pbrpc {
namespace server {

class RpcListener {
public:
    class MethodCallback {
    public:
        virtual ::google::protobuf::Service* 
            QueryService(const std::string& serviceName) = 0;

        virtual void OnMethodCall(
            ::google::protobuf::Service* service,
            const ::google::protobuf::MethodDescriptor* method,
            ::google::protobuf::RpcController* controller,
            const ::google::protobuf::Message* request,
            ::google::protobuf::Message* response,
            ::google::protobuf::Closure* done) = 0;
    };
public:
    virtual ~RpcListener() {}

    virtual Error GetMethodCallback(MethodCallback**) const = 0;

    virtual Error SetMethodCallback(MethodCallback* callback) = 0;

    virtual Error Start() = 0;

    virtual Error Stop() = 0;
};

}
}

#endif // _PBPRC_SERVER_RPC_LISTENER_H__