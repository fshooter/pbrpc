/*!
 * @file wintcprpclistener.h
 * @brief Windowsƽ̨��RPC�����������ļ�
 */
#ifndef _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__
#define _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__
#include <pbrpc/config.h>
#include <pbrpc/types.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

class WinTcpRpcListenerImpl;

/// @brief Windowsƽ̨��Tcp��ʽ��Rpc������ʵ��
///
/// �����ʵ���� WinTcpRpcListenerImpl ����
class WinTcpRpcListener : public RpcListener {
public:
    /// ���캯��
    WinTcpRpcListener();

    /// ��������
    ~WinTcpRpcListener();

    /// ��ʼ��
    Error Init(uint32_t ip, uint16_t port);

    /// ȡ�÷����ص�,�������ڷǿ���״̬�µ���
    Error GetMethodCallback(MethodCallback**) const;

    /// ���÷����ص�,�������ڷǿ���״̬�µ���
    Error SetMethodCallback(MethodCallback* callback);

    /// ����
    Error Start();

    /// �ر�
    Error Stop();
private:
    WinTcpRpcListenerImpl* impl_;
};

}
}

#endif // _PBRPC_SERVER_WIN_TCP_RPC_LISTENER_H__