#ifndef _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_IMPL_H__
#define _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_IMPL_H__
#include <Windows.h>
#include <vector>
#include <google/protobuf/service.h>
#include <pbrpc/config.h>
#include <pbrpc/error.h>
#include <pbrpc/types.h>
#include "simpletcpchannel.h"

namespace pbrpc {
namespace client {

using namespace ::google::protobuf;

class SimpleTcpChannel::Impl : public RpcChannel {

public:
    Impl();
    ~Impl();

    Error Init(uint32_t ip, uint16_t port);

    bool Bad();

    void CallMethod(const MethodDescriptor* method,
        RpcController* controller,
        const Message* request,
        Message* response,
        Closure* done);

private:

    void ChannelError();

private:
    SOCKET sock_;
    bool initialized_;
    int idCounter_;
    static bool isWsaStartupCalled_;
};

class SimpleTcpChannelPool::Impl {
    struct ChannelContext {
        SimpleTcpChannel* channel_;
        DWORD lastReleaseTime_;
    };
public:
    Impl();
    ~Impl();

    Error Init(uint32_t ip, uint16_t port);

    void SetMinChannelNum(int);

    void SetMaxChannelNum(int);

    void SetChannelTimeout(unsigned);

    SimpleTcpChannel* GetChannel();

    Error ReleaseChannel(SimpleTcpChannel*);

private:
    bool initialized_;
    uint32_t ip_;
    uint32_t port_;
    int minChannelNum_;
    int maxChannelNum_;
    int channelTimeout_;
    CRITICAL_SECTION lock_;
    typedef std::vector<ChannelContext*> ChannelContextVector;
    ChannelContextVector freeChannels_;
    ChannelContextVector busyChannels_;
};

}
}

#endif // _PBRPC_CLIENT_SIMPLE_TCP_CHANNEL_IMPL_H__