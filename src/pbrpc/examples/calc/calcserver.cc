#include <WinSock2.h>
#include <stdio.h>
#include <pbrpc./server/wintcprpclistener.h>
#include <pbrpc/server/rpcserver.h>
#include <pbrpc/types.h>
#include "calc.pb.h"
#include "../getopt.h"

using namespace ::google::protobuf;
using namespace ::pbrpc;
using namespace ::pbrpc::server;
using namespace ::pbrpc::examples::calc;

uint32_t host;
uint16_t port = 9999;
int threadNum = 1;

class CalcServiceImpl : public CalcService {
public:
    void Calc(::google::protobuf::RpcController* controller,
        const ::pbrpc::examples::calc::CalcRequest* request,
        ::pbrpc::examples::calc::CalcResponse* response,
        ::google::protobuf::Closure* done)
    {
        int result = 0;
        for (auto it = request->nums().begin();
            it != request->nums().end();
            ++it) {
            result += *it;
        }
        response->set_result(result);
        done->Run();
    }
};

int help() {
    return 0;
}

int main(int argc, char** argv) {

    host = inet_addr("0.0.0.0");

    int c;
    unsigned long tmp;
    while ((c = getopt(argc, argv, "?h:p:t:")) != -1) {
        switch (c) {
        case '?':
            return help();
        case 'h':
            host = inet_addr(optarg);
            if (host == INADDR_NONE) {
                printf("invalid host\n");
                return - 1;
            }
            break;
        case 'p':
            if (sscanf(optarg, "%d", &tmp) != 1 ||
                tmp >= INT16_MAX) {
                printf("invalid port\n");
                return -1;
            }
            port = (uint16_t)tmp;
            break;
        case 't':
            if (sscanf(optarg, "%d", &tmp) != 1) {
                printf("invalid thread num\n");
                return -1;
            }
            threadNum = tmp;
            break;
        }
    }

    CalcServiceImpl calcService;

    WinTcpRpcListener listener;
    if (listener.Init(host, port) != Error::E_OK) {
        printf("listener init failed\n");
        return -1;
    }

    RpcServer server;
    if (server.Init(&listener) != Error::E_OK) {
        printf("server init failed\n");
        return -1;
    }

    if (server.RegisterService(&calcService) != Error::E_OK) {
        printf("register service failed\n");
        return -1;
    }

    if (server.Start(threadNum) != Error::E_OK) {
        printf("server start failed\n");
        return -1;
    }

    printf("Press Enter To Exit\n");
    getchar();

    if (server.Stop() != Error::E_OK) {
        printf("stop server faield\n");
        return -1;
    }

    return 0;
}