/*!
 * @file rpclistener.h
 * @brief RPC�����������ļ�
 */
#ifndef _PBPRC_SERVER_RPC_LISTENER_H__
#define _PBPRC_SERVER_RPC_LISTENER_H__
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <pbrpc/config.h>
#include <pbrpc/error.h>

namespace pbrpc {
namespace server {

/// @brief RPC�������ӿ�
///
/// RPC�����������첽����RPC����,���յ�RPC����֮����� MethodCallback::OnMethodCall ֪ͨʹ����
/// RPC�����нӿ�Ӧ���Ƿ�������,ͨ��ʵ���߿���ѡ�����ڲ�ά��һ�������߳�����IO����
/// ͬ��,RPC�������ص� MethodCallback ��ʵ��ҲӦ���Ƿ�������
/// ���еľ���RPC������ʵ��,��TCP/UDP/HTTP��Ӧ��ʵ�ִ˽ӿ�
class RpcListener {
public:
    
    /// RPC֪ͨ�ص��ӿ�
    class MethodCallback {
    public:

        /// @brief �������Ʋ�ѯ����
        /// @retval ������� �� NULL
        /// ���� RpcListener �ڲ��̵߳���,ע��ͬ��,��Ҫ����
        virtual ::google::protobuf::Service* 
            QueryService(const std::string& serviceName) = 0;

        /// @brief RPC�ص�
        ///
        /// ���յ�RPC����ʱ,����ô˽ӿ�֪ͨ�ⲿʹ����.
        /// �˺����� RpcListener �ڲ��̵߳���,ע��ͬ��,��Ҫ����
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

    /// ȡ�ûص�����
    virtual Error GetMethodCallback(MethodCallback**) const = 0;

    /// ���ûص�����
    virtual Error SetMethodCallback(MethodCallback* callback) = 0;

    /// ����
    virtual Error Start() = 0;

    /// ֹͣ
    virtual Error Stop() = 0;
};

}
}

#endif // _PBPRC_SERVER_RPC_LISTENER_H__