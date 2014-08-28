/*!
 * @file rpclistener.h
 * @brief RPC监听器声明文件
 */
#ifndef _PBPRC_SERVER_RPC_LISTENER_H__
#define _PBPRC_SERVER_RPC_LISTENER_H__
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <pbrpc/config.h>
#include <pbrpc/error.h>

namespace pbrpc {
namespace server {

/// @brief RPC监听器接口
///
/// RPC监听器用于异步监听RPC调用,在收到RPC请求之后调用 MethodCallback::OnMethodCall 通知使用者
/// RPC的所有接口应当是非阻塞的,通常实现者可以选择在内部维护一个或多个线程用于IO处理
/// 同理,RPC监听器回调 MethodCallback 的实现也应当是非阻塞的
/// 所有的具体RPC监听器实现,如TCP/UDP/HTTP等应当实现此接口
class RpcListener {
public:
    
    /// RPC通知回调接口
    class MethodCallback {
    public:

        /// @brief 根据名称查询服务
        /// @retval 服务对象 或 NULL
        /// 会由 RpcListener 内部线程调用,注意同步,不要阻塞
        virtual ::google::protobuf::Service* 
            QueryService(const std::string& serviceName) = 0;

        /// @brief RPC回调
        ///
        /// 接收到RPC请求时,会调用此接口通知外部使用者.
        /// 此函数由 RpcListener 内部线程调用,注意同步,不要阻塞
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

    /// 取得回调对象
    virtual Error GetMethodCallback(MethodCallback**) const = 0;

    /// 设置回调对象
    virtual Error SetMethodCallback(MethodCallback* callback) = 0;

    /// 开启
    virtual Error Start() = 0;

    /// 停止
    virtual Error Stop() = 0;
};

}
}

#endif // _PBPRC_SERVER_RPC_LISTENER_H__