#include <WinSock2.h>
#include <stdio.h>
#include <google/protobuf/stubs/common.h>
#include <pbrpc/client/simpletcpchannel.h>
#include <pbrpc/types.h>
#include "calc.pb.h"
#include "../getopt.h"

using namespace ::pbrpc::examples::calc;
using namespace ::pbrpc;
using namespace ::pbrpc::client;
using namespace ::google::protobuf;

uint32_t host;
uint16_t port = 9999;

int help() {
    return 0;
}

void handleResponse() {

}

int main(int argc, char** argv) {

    if (argc == 1)
        return help();

    host = inet_addr("127.0.0.1");

    int c;
    unsigned long tmp;
    while ((c = getopt(argc, argv, "?h:p:")) != -1) {
        switch (c) {
        case '?':
            return help();
        case 'h':
            host = inet_addr(optarg);
            if (host == INADDR_NONE) {
                printf("invalid host\n");
                return -1;
            }
            break;
        case 'p':
            if (sscanf(optarg, "%d", &tmp) != 1 ||
                tmp >= 0x7FFF) {
                printf("invalid port\n");
                return -1;
            }
            port = (uint16_t)tmp;
            break;
        }
    }

    if (optind == argc) {
        printf("No Input Nums\n");
        return -1;
    }

    CalcRequest request;
    for (; optind < argc; ++optind) {
        if (sscanf(argv[optind], "%d", &tmp) != 1) {
            printf("invalid num\n");
            return -1;
        }
        request.add_nums(tmp);
    }

    SimpleTcpChannelPool channelPool;
    if (channelPool.Init(host, port) != Error::E_OK) {
        printf("channel pool init failed\n");
        return -1;
    }

    SimpleTcpChannel* channel = channelPool.GetChannel();
    if (!channel) {
        printf("GetChannel failed\n");
        return -1;
    }

    SimpleTcpRpcController controller;
    CalcService::Stub stub(channel);
    CalcResponse response;

    stub.Calc(&controller, &request, &response, NewCallback(handleResponse));

    if (controller.Failed()) {
        printf("rpc failed: %s\n", controller.ErrorText().c_str());
    }
    else {
        printf("result is %d\n", response.result());
    }

    channelPool.ReleaseChannel(channel);
    
    return 0;
}