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

/// @brief �򵥵�Tcpͨ��,����ʽ,���̰߳�ȫ
class SimpleTcpChannel : public ::google::protobuf::RpcChannel {
public:
    SimpleTcpChannel();
    ~SimpleTcpChannel();

    /// ��ʼ��
    Error Init(uint32_t ip, uint16_t port);

    /// ͨ���Ƿ�����
    bool Bad();

    /// @brief RpcChannel �ķ���ʵ��
    ///
    /// ��ͨ����Զ��ͬ������ʽ����, ���� done �ܻ��� CallMethod �����б�����
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
        ::google::protobuf::RpcController* controller,
        const ::google::protobuf::Message* request,
        ::google::protobuf::Message* response,
        ::google::protobuf::Closure* done);
private:
    class Impl;
    Impl* impl_;
};

/// @brief �򵥵�Tcp���ӳ�,�̰߳�ȫ
class SimpleTcpChannelPool {
public:
    SimpleTcpChannelPool();
    ~SimpleTcpChannelPool();

    /// ��ʼ��
    Error Init(uint32_t ip, uint16_t port);

    /// �������ٱ��ֵ�������, ���ڱ���Ŀ�����ӻ���Channelʹ����Ϻ������ر�
    void SetMinChannelNum(int);

    /// ������ౣ���������, �����ǰ���������ڱ���Ŀ,��֮��� GetChannel ���������ֱ��һ�����ӱ��ͷ�
    void SetMaxChannelNum(int);

    /// ����ͨ����ʱʱ��,���г������ʱ���ͨ���ᱻ�Զ��رղ�����ʹ��
    void SetChannelTimeout(unsigned);

    /// ȡ��һ����������
    SimpleTcpChannel* GetChannel();

    /// ʹ����Ϻ��ͷ�һ������,�����ӻ���ݲ��Ա������ڿ����б����ֱ�ӱ��ر�
    Error ReleaseChannel(SimpleTcpChannel*);
private:
    class Impl;
    Impl* impl_;
};

}
}

#endif // _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_H__