/*!
 * @file rpcserver.h
 * @brief RPC�����������ļ�
 */
#ifndef _PBRPC_SERVER_RPC_SERVER_H__
#define _PBRPC_SERVER_RPC_SERVER_H__
#include <pbrpc/config.h>
#include "rpclistener.h"

namespace pbrpc {
namespace server {

/// @brief Rpc������
///
/// ���๹���� RpcListener �ṩ�ļ�������֮��,
/// ͨ�� RpcListener �ṩ��Rpc����ص��Ƿ�������,�����ṩһ���̳߳�,����Rpc�������̳߳���ִ�о��幦��
/// �Ӷ�����ͬ��������ʽִ�з����ص�,�򻯷���ʵ���ߵ��Ѷ�
class RpcServer {
public:
    RpcServer();
    ~RpcServer();

    /// ��ʼ��
    Error Init(RpcListener* listener);

    /// ע�����,ֻ���ڷǿ���״̬�²��ܵ���
    Error RegisterService(::google::protobuf::Service* service);

    /// ��ע�����,ֻ���ڷǿ���״̬�²��ܵ���
    Error UnregisterServcie(::google::protobuf::Service* service);

    /// ����
    Error Start(int threadNum);

    /// �ر�
    Error Stop();
private:
    class Impl;
    Impl* impl_;
};

}
}

#endif // _PBRPC_SERVER_RPC_SERVER_H__