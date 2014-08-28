/*!
 * @file wintcprpclistenerimpl.h
 * @brief WinTcpRpcListenerImpl�������ļ�
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

/// @brief WinTcpRpcListener ��ʵ����
///
/// �����ڲ�ʹ�õ�IO�߳�+��ɶ˿�ʵ��.
/// ��IO�߳�ʡ����ͬ����,�����ڲ�ʵ��.
///
/// ��Ϣ�������ϰ�֡����,ÿ��֡����һ��������Rpc���������Ӧ,֡��ʽΪ:
///   [֡����4�ֽ�][֡��,��RequestMsg/ResponseMsg]
class WinTcpRpcListenerImpl : public RpcListener {

private:

    /// AcceptEx��������
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

    /// GetAcceptExSockaddrs��������
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

    /// @brief �ڲ�״̬
    ///
    /// ״̬ת��:
    ///                        Init              Start
    ///         UNINITIALIZED -------> PAUSED ----------> RUNNING
    ///                                   ^                  |
    ///                                   |                  | Stop
    ///                                   |                  V
    ///                               STOPPING_2 <------ STOPPING_1
    enum Status {
        UNINITIALIZED = 0, //!< δ��ʼ��
        PAUSED, //!< ��ͣ��
        RUNNING, //!< ������
        STOPPING_1, //!< ����ֹͣ,��ʱ��Ҫֹͣ����Accept���������Omc
        STOPPING_2, //!< ����ֹͣ,��ʱ��ҪֹͣIo�߳�
    };

    /// @brief Socket������
    ///
    /// ���� SocketContext ����������:
    ///   SocketContext��Tcp���ӳɹ�������(Accept�ɹ���)����,
    ///   ������Rpc������ɺ�(����� outstandingMethodCalls �б�Ϊ��ʱ)����
    struct SocketContext {
        SOCKET sock; //!< Socket���
        ListHead outstandingMethodCalls; //!< ��Socket��δ��ɵ�Rpc�����б�, ��Ӧ Omc::outstandingMethodCallsSocketEntry
    };

    /// Rpc������
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

    /// Omc ״̬
    enum OmcStatus {
        RECVING, //!< ����Rpc������
        PROCESSING, //!< ����Rpc������
        SENDING //!< ����Rpc�����
    };

    /// Omc�� RECVING ״̬ʱ����Ϣ
    struct OmcRecving {
        uint32_t requestSize; //!< Rpc����Ĵ�С,���Ϊ0,��ʾ��δ���յ���С
        std::string request; //!< Rpc�������ݻ�����
    };

    /// Omc�� PROCESSING ״̬ʱ����Ϣ
    struct OmcProcessing {
        int id; //!< Rpc����ID
        const Message* request; //!< Rpc Request
        Message* response; //!< Rpc Response
        SimpleRpcController controller; //!< Rpc������
    };

    /// Omc�� SENDING ״̬ʱ����Ϣ
    struct OmcSending {
        bool frameHeaderSended; //!< �Ƿ��Ѿ�������֡ͷ,֡ͷ=4�ֽ�֡����
        std::string response; //!< Rpc��Ӧ����
        uint32_t bytesLeftToSend; //!< response�л��ж����ֽ���Ҫ����
    };

    /// @brief ��δ������ϵ�Rpc����������, Omc = OutstandingMethodCall
    ///
    /// ÿһ��Rpc�������ҽ���һ�� Omc
    struct Omc {
        OmcStatus status; //!< ״̬
        SocketContext* socketContext; //!< �������Ӧ��Socket����
        ListHead outstandingMethodCallsEntry; //!< ���� SocketContext::outstandingMethodCalls
        ListHead outstandingMethodCallsSocketEntry; //!< ���� WinTcpRpcListenerImpl::outstandingMethodCalls_
        union {
            OmcRecving* recving; //!< �� status == RECVING ʱ��Ч
            OmcProcessing* processing; //!< �� status == PROCESSING ʱ��Ч
            OmcSending* sending; //!< �� status == SENDING ʱ��Ч
        } specific;
    };

    /// Io����
    enum IoType {
        ACCEPT,
        SEND,
        RECV,
        METHOD_CALL_DONE,
        LISTENER_STOPPING,
    };

    /// ÿIO������,����ɶ˿����ʹ��,ÿ���첽IO���󶼻���һ��������
    struct PerIoData {
        IoType type;
        OVERLAPPED overlapped;
    };

    /// Accept IO ������
    struct PerIoDataAccept : public PerIoData {
        SOCKET sock;
        uint8_t recvBuffer[(sizeof(sockaddr_in) + 16) * 2];
    };

    /// Send IO ������
    struct PerIoDataSend : public PerIoData {
        Omc* omc;
    };

    /// Recv IO ������
    struct PerIoDataRecv : public PerIoData {
        Omc* omc;
    };

    /// @brief Rpc���ý���
    ///
    /// ��Ϊ Rpc���õ� done �ص����ܷ������κ��߳���,
    /// ������ done �ص�����ҪͶ�ݱ���Ϣ�� Omc �Ĵ���ת�Ƶ�IO�߳�
    struct PerIoDataMethodCallDone : public PerIoData {
        Omc* omc;
    };

    /// @brief �ر�����
    ///
    /// ��Ϊ Stop ���ûᷢ���ڷ�IO�߳���,
    /// �����ҪͶ�ݱ���Ϣ�Ա���IO�߳�����ɱ�Ҫ�Ĺرղ���
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

    /// ����һ���첽��Accept����
    void DoAccept();

    /// Accept������ɻص�
    void OnAcceptFinish(bool success, PerIoDataAccept* acceptData);

    /// �� Omc �еĴ����������첽����
    void DoSend(Omc* omc);

    /// ������ɻص�
    void OnSendFinish(bool success, PerIoDataSend* sendData);

    /// ���ض�Socket���½� Omc �������첽Recv
    void DoRecv(SocketContext*);

    /// ��һ�� Omc �����첽����
    void DoRecv(Omc* omc);

    /// ������ɻص�
    void OnRecvFinish(bool success, PerIoDataRecv* recvData);

    /// �ر� Omc
    void CloseOmc(Omc*);

    /// �� Omc �����Rpc�������ʱ����,�̰߳�ȫ
    void OnMethodCallDoneThreadSafe(Omc*);

    /// �� IO �߳���ִ��,�յ�Rpc���������Ϣʱ����
    void OnMethodCallDone(PerIoDataMethodCallDone* doneData);

    /// �� IO �߳���ִ��,�յ�ֹͣ��Ϣʱ����
    void OnDataStopping(PerIoDataStopping* stopData);

    /// IO�̺߳���
    void WorkerThreadRoutine();

    /// IO�̺߳������
    static DWORD WINAPI WorkerThreadRoutineStub(LPVOID);

private:

    volatile Status status_; //!< ״̬

    uint32_t ip_; //!< ��Ҫ������IP
    uint16_t port_; //!< ��Ҫ�����Ķ˿�
    MethodCallback* methodCallback_; //!< �����ص�

    SOCKET sock_; //!< ������SOCKET
    HANDLE completionPort_; //!< ��ɶ˿�
    HANDLE thread_; //!< IO�߳�

    PfnAcceptEx acceptExRoutine_; //!< AcceptEx����ָ��
    PfnGetAcceptExSockaddrs getAcceptExSockaddrsRoutine_; //!< GetAcceptExSockaddrs����ָ��

    SOCKET currentAcceptSock_; //!< ��ǰ�����첽Accept��SOCKET,����ʱ��ֻ����һ��SOCKET��Accept, ֻ����IO�߳����޸�
    ListHead outstandingMethodCalls_; //!< ����δ��ɵ�Rpc����, ֻ����IO�߳����޸�
    size_t outstandingMethodCallsNum_; //!< ����δ��ɵ�Rpc��������, ֻ����IO�߳����޸�

    static bool isWsaStartupCalled_;
};

}
}

#endif // _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_IMPL_H__