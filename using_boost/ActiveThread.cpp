#include "ActiveThread.h"
#include <iostream>
#if defined(WIN32)
#include <windows.h>
#endif

unsigned get_native_thread_id(boost::thread& t)
{
#ifdef WIN32
    return GetThreadId(t.native_handle());
#else
    return t.get_id();
#endif // WIN32
}

ActiveThread::ActiveThread():thread_buffer_(1024*1024,1024),callback_(nullptr),begin_cb_(nullptr), timers_(nullptr)
{
    working_num_ = 0;
}

ActiveThread::~ActiveThread()
{
    if (timers_)
        delete timers_;
    timers_ = nullptr;
}

void ActiveThread::Start()
{
    thread_ = boost::thread(&ActiveThread::Run, this);    
    thread_id_ = get_native_thread_id(thread_);
    std::cout << "thread id:" << thread_id_ << " \n start!!! \n";
}

void ActiveThread::SetThreadStartCallback(BeginInThreadCallback begin_cb)
{
    begin_cb_ = begin_cb;
}

void ActiveThread::SetCallBack(Callback cb)
{
    callback_ = cb;
}

void ActiveThread::Send(buffer_base* pBuffer)
{
    boost::mutex::scoped_lock lock(mutex_);
    thread_buffer_.append(pBuffer);
}

void ActiveThread::Send(const char* pData, int n32Length)
{
    boost::mutex::scoped_lock lock(mutex_);
    thread_buffer_.append(n32Length);
    thread_buffer_.append(pData, n32Length);
}
 
void ActiveThread::Run()
{
    if (begin_cb_) {
        begin_cb_();
    }
    if (timers_)
    {
        timers_->start();
    }
    while (true) {
        Consume();
        if (timers_)
        {
            timers_->run_once();
        }
        Sleep(1);
    }
}

void ActiveThread::Consume() {
    boost::mutex::scoped_lock lock(mutex_);
    buffer_base* pBuffer = thread_buffer_.GetBuffer();
    while (pBuffer) {
        callback_(pBuffer);
        pBuffer = thread_buffer_.GetBuffer();
    }
}

ActiveThread*	ActiveThread::Create(Callback aCallBack)
{
    ActiveThread* aPtr = new ActiveThread();
    aPtr->callback_ = aCallBack;    
    return aPtr;
}

void ActiveThread::AddTimer(HeartbeatCallback pHeartbeatCallback, __int64 interval, eTType type, bool ifpersist)
{
    if (!timers_)
        timers_ = new timers();
    timers_->addtimer(pHeartbeatCallback, interval, type, ifpersist);
}