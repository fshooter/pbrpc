#include <pbrpc/config.h>
#include "simpletcpchannel.h"
#include "simpletcpchannelimpl.h"

namespace pbrpc {
namespace client {

SimpleTcpChannel::SimpleTcpChannel() {
    impl_ = new Impl();
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



SimpleTcpChannelPool::SimpleTcpChannelPool()
{
    impl_ = new Impl();
}

SimpleTcpChannelPool::~SimpleTcpChannelPool()
{
    delete impl_;
}

Error SimpleTcpChannelPool::Init(uint32_t ip, uint16_t port)
{
    return impl_->Init(ip, port);
}

void SimpleTcpChannelPool::SetMinChannelNum(int n)
{
    return impl_->SetMinChannelNum(n);
}

void SimpleTcpChannelPool::SetMaxChannelNum(int n)
{
    return impl_->SetMaxChannelNum(n);
}

void SimpleTcpChannelPool::SetChannelTimeout(unsigned t)
{
    return impl_->SetChannelTimeout(t);
}

SimpleTcpChannel* SimpleTcpChannelPool::GetChannel()
{
    return impl_->GetChannel();
}

Error SimpleTcpChannelPool::ReleaseChannel(SimpleTcpChannel* c)
{
    return impl_->ReleaseChannel(c);
}

}
}