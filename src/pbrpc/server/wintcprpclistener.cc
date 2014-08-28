/*!
 * @file wintcprpclistener.h
 * @brief Windowsƽ̨��RPC������ʵ���ļ�
 */
#include <pbrpc/config.h>
#include "wintcprpclistener.h"
#include "wintcprpclistenerimpl.h"

namespace pbrpc {
namespace server {

WinTcpRpcListener::WinTcpRpcListener()
{
    impl_ = new WinTcpRpcListenerImpl();
}

WinTcpRpcListener::~WinTcpRpcListener()
{
    delete impl_;
}

Error WinTcpRpcListener::Init(uint32_t ip, uint16_t port)
{
    return impl_->Init(ip, port);
}

Error WinTcpRpcListener::GetMethodCallback(MethodCallback** cb) const
{
    return impl_->GetMethodCallback(cb);
}

Error WinTcpRpcListener::SetMethodCallback(MethodCallback* callback)
{
    return impl_->SetMethodCallback(callback);
}

Error WinTcpRpcListener::Start()
{
    return impl_->Start();
}

Error WinTcpRpcListener::Stop()
{
    return impl_->Stop();
}

}
}