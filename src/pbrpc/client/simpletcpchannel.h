#ifndef _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__
#define _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__
#include <google/protobuf/service.h>
#include <pbrpc/config.h>
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

/// @brief 简单的Tcp通道,阻塞式,非线程安全
class SimpleTcpChannel : public ::google::protobuf::RpcChannel {
public:
    SimpleTcpChannel();
    ~SimpleTcpChannel();

    /// 初始化
    Error Init(uint32_t ip, uint16_t port);

    /// 通道是否已损坏
    bool Bad();

    /// @brief RpcChannel 的方法实现
    ///
    /// 本通道永远是同步阻塞式调用, 所以 done 总会在 CallMethod 方法中被调用
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
        ::google::protobuf::RpcController* controller,
        const ::google::protobuf::Message* request,
        ::google::protobuf::Message* response,
        ::google::protobuf::Closure* done);
private:
    class Impl;
    Impl* impl_;
};

/// @brief 简单的Tcp连接池,线程安全
class SimpleTcpChannelPool {
public:
    SimpleTcpChannelPool();
    ~SimpleTcpChannelPool();

    /// 初始化
    Error Init(uint32_t ip, uint16_t port);

    /// 设置最少保持的连接数, 多于本数目的连接会在Channel使用完毕后立即关闭
    void SetMinChannelNum(int);

    /// 设置最多保存的连接数, 如果当前连接数多于本数目,则之后的 GetChannel 请求会阻塞直到一个连接被释放
    void SetMaxChannelNum(int);

    /// 设置通道超时时间,空闲超过这个时间的通道会被自动关闭不会再使用
    void SetChannelTimeout(unsigned);

    /// 取得一个空闲连接
    SimpleTcpChannel* GetChannel();

    /// 使用完毕后释放一个连接,该连接会根据策略被保存在空闲列表或者直接被关闭
    Error ReleaseChannel(SimpleTcpChannel*);
private:
    class Impl;
    Impl* impl_;
};

}
}

#endif // _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__