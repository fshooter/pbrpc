/*!
 * @file rpcserver.h
 * @brief RPC服务器实现文件
 */
#include <Windows.h>
#include <map>
#include <google/protobuf/service.h>
#include <pbrpc/config.h>
#include <pbrpc/common/listhead.h>
#include <pbrpc/common/log.h>
#include <pbrpc/types.h>
#include "rpcserver.h"

#define MAX_PENDING_METHODCALL 5000

namespace pbrpc {
namespace server {

using namespace ::google::protobuf;
using namespace ::pbrpc::common;

/// @brief Rpc服务器实现类
///
/// 本类维护一个线程池,一旦接收到 RpcListener 的Rpc回调,
/// 便生成一个MethodCallCtx放入到请求队列中,线程池中的线程不断从请求队列中取出请求并执行
/// @note 使用者不应当使用此类,使用 RpcServer
class RpcServer::Impl : public RpcListener::MethodCallback {

    /// 回调请求上下文,目前只是保存了请求回调的各个参数
    struct MethodCallCtx {
        ListHead next;
        RpcController* controller;
        Service* service;
        const MethodDescriptor* methodDesc;
        const Message* request;
        Message* response;
        Closure* done;
    };

    /// RpcServer的状态
    enum Status {
        UNINITIALIIED, //!< 未初始化
        PAUSED, //!< 暂停中
        RUNNING //!< 运行中
    };
public:
    Impl();
    ~Impl();

    Error Init(RpcListener* listener);

    Error RegisterService(Service* service);

    Error UnregisterServcie(Service* service);

    Error Start(int threadNum);

    Error Stop();

    Service* QueryService(const std::string& serviceName);

    void OnMethodCall(
        Service* service,
        const MethodDescriptor* method,
        RpcController* controller,
        const Message* request,
        Message* response,
        Closure* done);

private:

    void OnDone(MethodCallCtx*);

    void ThreadRoutine();

    static DWORD WINAPI ThreadRoutineStub(LPVOID self);

private:
    Status status_;

    volatile bool exitThread_;
    int threadNum_;
    std::vector<HANDLE> threads_;

    RpcListener* listener_;

    typedef std::map<std::string, Service*> ServiceMap;
    ServiceMap services_;
    CRITICAL_SECTION servicesLock_;

    HANDLE methodCallsSemaphore_;
    CRITICAL_SECTION methodCallsLock_;
    ListHead methodCalls_;
};

RpcServer::Impl::Impl() : status_(Status::UNINITIALIIED)
{
}

RpcServer::Impl::~Impl() {
    if (status_ == Status::RUNNING) {
        if (Stop() != Error::E_OK)
            Fetal("RpcServer::Impl Error In Destructor\n");
    }
    if (status_ == Status::PAUSED) {
        CloseHandle(methodCallsSemaphore_);
        DeleteCriticalSection(&methodCallsLock_);
        DeleteCriticalSection(&servicesLock_);
    }
    assert(ListHeadEmpty(&methodCalls_));
}

Error RpcServer::Impl::Init(RpcListener* listener)
{
    if (status_ != Status::UNINITIALIIED)
        return Error::E_INVALID_STATUS;

    Error err;

    exitThread_ = false;
    ListHeadInitailize(&methodCalls_);
    
    listener_ = listener;

    err = listener_->SetMethodCallback(this);
    if (err != Error::E_OK)
        return err;

    methodCallsSemaphore_ = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
    if (!methodCallsSemaphore_) {
        listener_->SetMethodCallback(NULL);
        return Error::E_WIN32_ERR;
    }

    InitializeCriticalSection(&methodCallsLock_);
    InitializeCriticalSection(&servicesLock_);

    status_ = Status::PAUSED;

    return Error::E_OK;
}

Error RpcServer::Impl::RegisterService(Service* service) 
{
    if (status_ != Status::PAUSED)
        return Error::E_INVALID_STATUS;

    services_[service->GetDescriptor()->name()] = service;
    return Error::E_OK;
}

Error RpcServer::Impl::UnregisterServcie(Service* service)
{
    if (status_ != Status::PAUSED)
        return Error::E_INVALID_STATUS;

    services_.erase(service->GetDescriptor()->full_name());
    return Error::E_OK;
}

Error RpcServer::Impl::Start(int threadNum)
{
    if (status_ != Status::PAUSED)
        return Error::E_INVALID_STATUS;
    if (!threadNum)
        return Error::E_INVALID_PARAMETER;

    threadNum_ = threadNum;
    threads_.resize(threadNum_);
    for (int i = 0; i < threadNum_; ++i) {
        threads_[i] = CreateThread(NULL, 0, &RpcServer::Impl::ThreadRoutineStub, this, 0, NULL);
        if (!threads_[i])
            Fetal("Create Thread Error\n");
    }

    Error err = listener_->Start();
    if (err != Error::E_OK) {
        exitThread_ = true;
        ReleaseSemaphore(methodCallsSemaphore_, threadNum_, NULL);
        WaitForMultipleObjects(threadNum_, &threads_[0], TRUE, INFINITE);
        exitThread_ = false;
        for (int i = 0; i < threadNum_; ++i)
            CloseHandle(threads_[i]);
        return err;
    }

    status_ = Status::RUNNING;

    return Error::E_OK;
}

Error RpcServer::Impl::Stop() {

    if (status_ != Status::RUNNING)
        return Error::E_INVALID_STATUS;

    Error err = listener_->Stop();
    if (err != Error::E_OK)
        return err;

    exitThread_ = true;
    ReleaseSemaphore(methodCallsSemaphore_, threadNum_, NULL);
    WaitForMultipleObjects(threadNum_, &threads_[0], TRUE, INFINITE);
    exitThread_ = false;
    for (int i = 0; i < threadNum_; ++i)
        CloseHandle(threads_[i]);

    status_ = Status::PAUSED;

    return Error::E_OK;
}

Service* RpcServer::Impl::QueryService(const std::string& serviceName)
{
    ServiceMap::iterator it = services_.find(serviceName);
    if (it == services_.end())
        return NULL;
    return it->second;
}

void RpcServer::Impl::OnMethodCall(
    Service* service,
    const MethodDescriptor* method,
    RpcController* controller,
    const Message* request,
    Message* response,
    Closure* done)
{
    MethodCallCtx* methodCallCtx = new MethodCallCtx;
    assert(methodCallCtx);

    methodCallCtx->controller = controller;
    methodCallCtx->service = service;
    methodCallCtx->methodDesc = method;
    methodCallCtx->request = request;
    methodCallCtx->response = response;
    methodCallCtx->done = done;

    EnterCriticalSection(&methodCallsLock_);
    ListHeadInsertTail(&methodCalls_, &methodCallCtx->next);
    LeaveCriticalSection(&methodCallsLock_);
    if (!ReleaseSemaphore(methodCallsSemaphore_, 1, NULL))
        Fetal("Error");
}

void RpcServer::Impl::OnDone(MethodCallCtx* ctx)
{
    ctx->done->Run();
    delete ctx;
}

void RpcServer::Impl::ThreadRoutine()
{
    while (!exitThread_) {

        if (WaitForSingleObject(methodCallsSemaphore_, INFINITE) != WAIT_OBJECT_0)
            continue;

        if (exitThread_)
            break;

        EnterCriticalSection(&methodCallsLock_);
        ListHead* next = methodCalls_.next;
        if (next != &methodCalls_)
            ListHeadRemove(next);
        LeaveCriticalSection(&methodCallsLock_);

        if (next == &methodCalls_)
            continue;

        MethodCallCtx* ctx = CONTAINING_RECORD(next, MethodCallCtx, next);
        ctx->service->CallMethod(ctx->methodDesc,
            ctx->controller,
            ctx->request,
            ctx->response,
            NewCallback(this, &RpcServer::Impl::OnDone, ctx));
    }
}

DWORD WINAPI RpcServer::Impl::ThreadRoutineStub(LPVOID self)
{
    ((RpcServer::Impl*)self)->ThreadRoutine();
    return 0;
}

RpcServer::RpcServer()
{
    impl_ = new Impl();
}

RpcServer::~RpcServer() {
    delete impl_;
}

Error RpcServer::Init(RpcListener* listener)
{
    return impl_->Init(listener);
}

Error RpcServer::RegisterService(::google::protobuf::Service* service)
{
    return impl_->RegisterService(service);
}

Error RpcServer::UnregisterServcie(::google::protobuf::Service* service)
{
    return impl_->UnregisterServcie(service);
}

Error RpcServer::Start(int threadNum)
{
    return impl_->Start(threadNum);
}

Error RpcServer::Stop()
{
    return impl_->Stop();
}

}
}