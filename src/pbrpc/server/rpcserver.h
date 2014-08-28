/*!
 * @file rpcserver.h
 * @brief RPC服务器声明文件
 */
#ifndef _PBRPC_SERVER_RPC_SERVER_H__
#define _PBRPC_SERVER_RPC_SERVER_H__
#include <pbrpc/config.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

/// @brief Rpc服务器
///
/// 本类构建在 RpcListener 提供的监听功能之上,
/// 通常 RpcListener 提供的Rpc请求回调是非阻塞的,本类提供一个线程池,接收Rpc请求并在线程池中执行具体功能
/// 从而可以同步阻塞方式执行方法回调,简化服务实现者的难度
class RpcServer {
public:
    RpcServer();
    ~RpcServer();

    /// 初始化
    Error Init(RpcListener* listener);

    /// 注册服务,只有在非开启状态下才能调用
    Error RegisterService(::google::protobuf::Service* service);

    /// 反注册服务,只有在非开启状态下才能调用
    Error UnregisterServcie(::google::protobuf::Service* service);

    /// 开启
    Error Start(int threadNum);

    /// 关闭
    Error Stop();
private:
    class Impl;
    Impl* impl_;
};

}
}

#endif // _PBRPC_SERVER_RPC_SERVER_H__