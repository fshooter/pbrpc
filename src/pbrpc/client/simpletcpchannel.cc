#include "simpletcpchannel.h"
#include "simpletcpchannelimpl.h"

namespace pbrpc {
namespace client {

SimpleTcpChannel::SimpleTcpChannel() {
    impl_ = new SimpleTcpChannelImpl();
}

SimpleTcpChannel::~SimpleTcpChannel() {
    delete impl_;
}

Error SimpleTcpChannel::Init(uint32_t ip, uint16_t port) {
    return impl_->Init(ip, port);
}

bool SimpleTcpChannel::Bad()
{
    return impl_->Bad();
}

void SimpleTcpChannel::CallMethod(const MethodDescriptor* method,
    RpcController* controller,
    const Message* request,
    Message* response,
    Closure* done)
{
    return impl_->CallMethod(method, controller, request, response, done);
}

}
}