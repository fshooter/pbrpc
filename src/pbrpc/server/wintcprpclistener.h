#ifndef _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__
#define _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__
#include <pbrpc/types.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

class WinTcpRpcListenerImpl;

class WinTcpRpcListener : public RpcListener {
public:
    WinTcpRpcListener();
    ~WinTcpRpcListener();

    Error Init(uint32_t ip, uint16_t port);

    Error GetMethodCallback(MethodCallback**) const;

    Error SetMethodCallback(MethodCallback* callback);

    Error Start();

    Error Stop();
private:
    WinTcpRpcListenerImpl* impl_;
};

}
}

#endif // _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__