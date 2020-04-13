#include "ActiveThread.h"
#include <windows.h>
#include <iostream>

unsigned get_native_thread_id(boost::thread& t)
{
    return GetThreadId(t.native_handle());
}

ActiveThread::ActiveThread():thread_buffer_(1024*1024,1024),callback_(nullptr),begin_cb_(nullptr)
{
    working_num_ = 0;
}

ActiveThread::~ActiveThread()
{
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
    
    while (true) {
        Consume();
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