#ifndef _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__
#define _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__
#include <Windows.h>
#include <cassert>
#include <google/protobuf/service.h>
#include <pbrpc/types.h>
#include <pbrpc/common/listhead.h>
#include <pbrpc/proto/message.pb.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

using namespace ::pbrpc::common;
using namespace ::pbrpc::proto;
using namespace ::google::protobuf;

class WinTcpRpcListenerImpl : public RpcListener {

private:

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

    enum Status {
        UNINITIALIZED = 0,
        PAUSED, //!< 暂停中
        RUNNING, //!< 运行中
        STOPPING_1, //!< 正在停止,此时需要等待停止所有Accept并完成所有Omc
        STOPPING_2, //!< 正在停止,此时需要等待停止Io线程
    };

    struct SocketContext {
        SOCKET sock;
        ListHead outstandingMethodCalls;
    };

    class SimpleRpcController : public RpcController {
    public:
        SimpleRpcController() : failed_(false) {}

        void Reset() { assert(false); }
        bool Failed() const { return failed_; }
        std::string ErrorText() const { return reason_; }
        void StartCancel() { assert(false); }
        void SetFailed(const std::string& reason) {
            failed_ = true;
            reason_ = reason;
        }
        bool IsCanceled() const { return false; }
        void NotifyOnCancel(Closure* callback) { return; }
    private:
        bool failed_;
        std::string reason_;
    };

    enum OmcStatus {
        RECVING,
        PROCESSING,
        SENDING
    };

    struct OmcRecving {
        uint32_t requestSize;
        std::string request;
    };

    struct OmcProcessing {
        int id;
        const Message* request;
        Message* response;
        SimpleRpcController controller;
    };

    struct OmcSending {
        bool frameHeaderSended;
        std::string response;
        uint32_t bytesLeftToSend;
    };

    /// @brief OutstandingMethodCall
    struct Omc {
        OmcStatus status;
        SocketContext* socketContext;
        ListHead outstandingMethodCallsEntry;
        ListHead outstandingMethodCallsSocketEntry;
        union {
            OmcRecving* recving;
            OmcProcessing* processing;
            OmcSending* sending;
        } specific;
    };

    enum IoType {
        ACCEPT,
        SEND,
        RECV,
        METHOD_CALL_DONE,
        LISTENER_STOPPING,
    };

    struct PerIoData {
        IoType type;
        OVERLAPPED overlapped;
    };

    struct PerIoDataAccept : public PerIoData {
        SOCKET sock;
        uint8_t recvBuffer[(sizeof(sockaddr_in) + 16) * 2];
    };

    struct PerIoDataSend : public PerIoData {
        Omc* omc;
    };

    struct PerIoDataRecv : public PerIoData {
        Omc* omc;
    };

    struct PerIoDataMethodCallDone : public PerIoData {
        Omc* omc;
    };

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

    void DoAccept();

    void OnAcceptFinish(bool success, PerIoDataAccept* acceptData);

    void DoSend(Omc* omc);

    void OnSendFinish(bool success, PerIoDataSend* sendData);

    void DoRecv(SocketContext*);

    void DoRecv(Omc* omc);

    void OnRecvFinish(bool success, PerIoDataRecv* recvData);

    void CloseOmc(Omc*);

    void OnMethodCallDoneThreadSafe(Omc*);

    void OnMethodCallDone(PerIoDataMethodCallDone* doneData);

    void OnDataStopping(PerIoDataStopping* stopData);

    void WorkerThreadRoutine();

    static DWORD WINAPI WorkerThreadRoutineStub(LPVOID);

private:

    volatile Status status_;

    uint32_t ip_;
    uint16_t port_;
    MethodCallback* methodCallback_;

    SOCKET sock_;
    HANDLE completionPort_;
    HANDLE thread_;

    PfnAcceptEx acceptExRoutine_;
    PfnGetAcceptExSockaddrs getAcceptExSockaddrsRoutine_;

    SOCKET currentAcceptSock_;
    ListHead outstandingMethodCalls_;
    size_t outstandingMethodCallsNum_;
};

}
}

#endif // _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__