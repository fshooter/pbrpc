#ifndef _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_IMPL_H__
#define _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_IMPL_H__
#include <Windows.h>
#include <google/protobuf/service.h>
#include <pbrpc/error.h>
#include <pbrpc/types.h>

namespace pbrpc {
namespace client {

using namespace ::google::protobuf;

class SimpleTcpChannelImpl : public RpcChannel {

public:
    SimpleTcpChannelImpl();
    ~SimpleTcpChannelImpl();

    Error Init(uint32_t ip, uint16_t port);

    bool Bad();

    void CallMethod(const MethodDescriptor* method,
        RpcController* controller,
        const Message* request,
        Message* response,
        Closure* done);

private:

    void ChannelError();

private:
    SOCKET sock_;
    bool initialized_;
    int idCounter_;
};

}
}

#endif // _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_IMPL_H__