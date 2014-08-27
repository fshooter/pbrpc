#ifndef _PBRPC_SERVER_RPC_SERVER_H__
#define _PBRPC_SERVER_RPC_SERVER_H__
#include "rpclistener.h"

namespace pbrpc {
namespace server {

class RpcServer {
public:
    RpcServer();
    ~RpcServer();

    Error Init(RpcListener* listener);

    Error RegisterService(::google::protobuf::Service* service);

    Error UnregisterServcie(::google::protobuf::Service* service);

    Error Start(int threadNum);

    Error Stop();
private:
    class Impl;
    Impl* impl_;
};

}
}

#endif // _PBRPC_SERVER_RPC_SERVER_H__