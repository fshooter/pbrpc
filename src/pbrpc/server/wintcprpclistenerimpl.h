/*!
 * @file wintcprpclistenerimpl.h
 * @brief WinTcpRpcListenerImpl类声明文件
 */
#ifndef _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__
#define _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__
#include <Windows.h>
#include <cassert>
#include <google/protobuf/service.h>
#include <pbrpc/config.h>
#include <pbrpc/types.h>
#include <pbrpc/common/listhead.h>
#include <pbrpc/proto/message.pb.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

using namespace ::pbrpc::common;
using namespace ::pbrpc::proto;
using namespace ::google::protobuf;

/// @brief WinTcpRpcListener 的实现类
///
/// 本类内部使用单IO线程+完成端口实现.
/// 单IO线程省掉了同步锁,简化了内部实现.
///
/// 消息在网络上按帧传输,每个帧包含一个完整的Rpc请求或者响应,帧格式为:
///   [帧长度4字节][帧体,即RequestMsg/ResponseMsg]
class WinTcpRpcListenerImpl : public RpcListener {

private:

    /// AcceptEx函数声明
    typedef BOOL(WINAPI *PfnAcceptEx)(
        _In_   SOCKET sListenSocket,
        _In_   SOCKET sAcceptSocket,
        _In_   PVOID lpOutputBuffer,
        _In_   DWORD dwReceiveDataLength,
        _In_   DWORD dwLocalAddressLength,
        _In_   DWORD dwRemoteAddressLength,
        _Out_  LPDWORD lpdwBytesReceived,
        _In_   LPOVERLAPPED lpOverlapped
        );

    /// GetAcceptExSockaddrs函数声明
    typedef void (WINAPI *PfnGetAcceptExSockaddrs)(
        _In_   PVOID lpOutputBuffer,
        _In_   DWORD dwReceiveDataLength,
        _In_   DWORD dwLocalAddressLength,
        _In_   DWORD dwRemoteAddressLength,
        _Out_  LPSOCKADDR *LocalSockaddr,
        _Out_  LPINT LocalSockaddrLength,
        _Out_  LPSOCKADDR *RemoteSockaddr,
        _Out_  LPINT RemoteSockaddrLength
        );

    /// @brief 内部状态
    ///
    /// 状态转换:
    ///                        Init              Start
    ///         UNINITIALIZED -------> PAUSED ----------> RUNNING
    ///                                   ^                  |
    ///                                   |                  | Stop
    ///                                   |                  V
    ///                               STOPPING_2 <------ STOPPING_1
    enum Status {
        UNINITIALIZED = 0, //!< 未初始化
        PAUSED, //!< 暂停中
        RUNNING, //!< 运行中
        STOPPING_1, //!< 正在停止,此时需要停止所有Accept并完成所有Omc
        STOPPING_2, //!< 正在停止,此时需要停止Io线程
    };

    /// @brief Socket上下文
    ///
    /// 关于 SocketContext 的生命周期:
    ///   SocketContext在Tcp连接成功建立后(Accept成功后)创建,
    ///   在所有Rpc调用完成后(自身的 outstandingMethodCalls 列表为空时)销毁
    struct SocketContext {
        SOCKET sock; //!< Socket句柄
        ListHead outstandingMethodCalls; //!< 该Socket上未完成的Rpc调用列表, 对应 Omc::outstandingMethodCallsSocketEntry
    };

    /// Rpc控制器
    class SimpleRpcController : public RpcController {
    public:
        SimpleRpcController() : failed_(false), canceled_(false), cancelCallback_(NULL) {}

        void Reset() { assert(false); }
        bool Failed() const { return failed_; }
        std::string ErrorText() const { return reason_; }
        void StartCancel() { canceled_ = true; if (cancelCallback_) { cancelCallback_->Run(); } }
        void SetFailed(const std::string& reason) {
            failed_ = true;
            reason_ = reason;
        }
        bool IsCanceled() const { return canceled_; }
        void NotifyOnCancel(Closure* callback) { cancelCallback_ = callback; }
    private:
        bool canceled_;
        Closure* cancelCallback_;
        bool failed_;
        std::string reason_;
    };

    /// Omc 状态
    enum OmcStatus {
        RECVING, //!< 接收Rpc请求中
        PROCESSING, //!< 处理Rpc请求中
        SENDING //!< 发送Rpc结果中
    };

    /// Omc在 RECVING 状态时的信息
    struct OmcRecving {
        uint32_t requestSize; //!< Rpc请求的大小,如果为0,表示尚未接收到大小
        std::string request; //!< Rpc请求内容缓冲区
    };

    /// Omc在 PROCESSING 状态时的信息
    struct OmcProcessing {
        int id; //!< Rpc请求ID
        const Message* request; //!< Rpc Request
        Message* response; //!< Rpc Response
        SimpleRpcController controller; //!< Rpc控制器
    };

    /// Omc在 SENDING 状态时的信息
    struct OmcSending {
        bool frameHeaderSended; //!< 是否已经发送了帧头,帧头=4字节帧长度
        std::string response; //!< Rpc响应内容
        uint32_t bytesLeftToSend; //!< response中还有多少字节需要发送
    };

    /// @brief 尚未处理完毕的Rpc请求上下文, Omc = OutstandingMethodCall
    ///
    /// 每一个Rpc请求有且仅有一个 Omc
    struct Omc {
        OmcStatus status; //!< 状态
        SocketContext* socketContext; //!< 该请求对应的Socket连接
        ListHead outstandingMethodCallsEntry; //!< 用于 SocketContext::outstandingMethodCalls
        ListHead outstandingMethodCallsSocketEntry; //!< 用于 WinTcpRpcListenerImpl::outstandingMethodCalls_
        union {
            OmcRecving* recving; //!< 在 status == RECVING 时有效
            OmcProcessing* processing; //!< 在 status == PROCESSING 时有效
            OmcSending* sending; //!< 在 status == SENDING 时有效
        } specific;
    };

    /// Io类型
    enum IoType {
        ACCEPT,
        SEND,
        RECV,
        METHOD_CALL_DONE,
        LISTENER_STOPPING,
    };

    /// 每IO上下文,和完成端口配合使用,每个异步IO请求都会有一个本对象
    struct PerIoData {
        IoType type;
        OVERLAPPED overlapped;
    };

    /// Accept IO 上下文
    struct PerIoDataAccept : public PerIoData {
        SOCKET sock;
        uint8_t recvBuffer[(sizeof(sockaddr_in) + 16) * 2];
    };

    /// Send IO 上下文
    struct PerIoDataSend : public PerIoData {
        Omc* omc;
    };

    /// Recv IO 上下文
    struct PerIoDataRecv : public PerIoData {
        Omc* omc;
    };

    /// @brief Rpc调用结束
    ///
    /// 因为 Rpc调用的 done 回调可能发生在任何线程中,
    /// 所以在 done 回调中需要投递本消息将 Omc 的处理转移到IO线程
    struct PerIoDataMethodCallDone : public PerIoData {
        Omc* omc;
    };

    /// @brief 关闭自身
    ///
    /// 因为 Stop 调用会发生在非IO线程中,
    /// 因此需要投递本消息以便在IO线程中完成必要的关闭操作
    struct PerIoDataStopping : public PerIoData {
    };

public:

    WinTcpRpcListenerImpl();

    ~WinTcpRpcListenerImpl();

    Error Init(uint32_t ip, uint16_t port);

    Error GetMethodCallback(MethodCallback**) const;

    Error SetMethodCallback(MethodCallback* callback);

    Error Start();

    Error Stop();

private:

    /// 发起一个异步的Accept请求
    void DoAccept();

    /// Accept请求完成回调
    void OnAcceptFinish(bool success, PerIoDataAccept* acceptData);

    /// 将 Omc 中的待发送数据异步发送
    void DoSend(Omc* omc);

    /// 发送完成回调
    void OnSendFinish(bool success, PerIoDataSend* sendData);

    /// 在特定Socket上新建 Omc 并进行异步Recv
    void DoRecv(SocketContext*);

    /// 对一个 Omc 进行异步接收
    void DoRecv(Omc* omc);

    /// 接收完成回调
    void OnRecvFinish(bool success, PerIoDataRecv* recvData);

    /// 关闭 Omc
    void CloseOmc(Omc*);

    /// 当 Omc 代表的Rpc请求完成时调用,线程安全
    void OnMethodCallDoneThreadSafe(Omc*);

    /// 在 IO 线程中执行,收到Rpc调用完成消息时调用
    void OnMethodCallDone(PerIoDataMethodCallDone* doneData);

    /// 在 IO 线程中执行,收到停止消息时调用
    void OnDataStopping(PerIoDataStopping* stopData);

    /// IO线程函数
    void WorkerThreadRoutine();

    /// IO线程函数存根
    static DWORD WINAPI WorkerThreadRoutineStub(LPVOID);

private:

    volatile Status status_; //!< 状态

    uint32_t ip_; //!< 需要监听的IP
    uint16_t port_; //!< 需要监听的端口
    MethodCallback* methodCallback_; //!< 方法回调

    SOCKET sock_; //!< 监听的SOCKET
    HANDLE completionPort_; //!< 完成端口
    HANDLE thread_; //!< IO线程

    PfnAcceptEx acceptExRoutine_; //!< AcceptEx函数指针
    PfnGetAcceptExSockaddrs getAcceptExSockaddrsRoutine_; //!< GetAcceptExSockaddrs函数指针

    SOCKET currentAcceptSock_; //!< 当前正在异步Accept的SOCKET,任意时刻只会有一个SOCKET在Accept, 只能在IO线程中修改
    ListHead outstandingMethodCalls_; //!< 所有未完成的Rpc调用, 只能在IO线程中修改
    size_t outstandingMethodCallsNum_; //!< 所有未完成的Rpc调用数量, 只能在IO线程中修改

    static bool isWsaStartupCalled_;
};

}
}

#endif // _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__