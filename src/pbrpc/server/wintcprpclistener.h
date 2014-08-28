/*!
 * @file wintcprpclistener.h
 * @brief Windows平台下RPC监听器声明文件
 */
#ifndef _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__
#define _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__
#include <pbrpc/config.h>
#include <pbrpc/types.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

class WinTcpRpcListenerImpl;

/// @brief Windows平台下Tcp方式的Rpc监听器实现
///
/// 具体的实现在 WinTcpRpcListenerImpl 类中
class WinTcpRpcListener : public RpcListener {
public:
    /// 构造函数
    WinTcpRpcListener();

    /// 析构函数
    ~WinTcpRpcListener();

    /// 初始化
    Error Init(uint32_t ip, uint16_t port);

    /// 取得方法回调,仅可以在非开启状态下调用
    Error GetMethodCallback(MethodCallback**) const;

    /// 设置方法回调,仅可以在非开启状态下调用
    Error SetMethodCallback(MethodCallback* callback);

    /// 开启
    Error Start();

    /// 关闭
    Error Stop();
private:
    WinTcpRpcListenerImpl* impl_;
};

}
}

#endif // _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__