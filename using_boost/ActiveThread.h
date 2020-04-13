#ifndef _ACTIVE_THREAD_H_
#define _ACTIVE_THREAD_H_
#include <functional>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "inc\thread_buffer.h"

typedef std::function<void()> BeginInThreadCallback;
typedef std::function<void(buffer_base*&)> Callback;
class ActiveThread
{
public:
    ActiveThread();
    ~ActiveThread();

    void Run();
    void Consume();
    static ActiveThread*	Create(Callback aCallBack);
    void			Start();
    void			SetCallBack(Callback cb);
    void			SetThreadStartCallback(BeginInThreadCallback begin_cb);
    void			Send(buffer_base* pBuffer);
    void			Send(const char* pData,int un32Length);
private:
    BeginInThreadCallback                  begin_cb_;
    boost::thread	                       thread_;
    boost::mutex	                       mutex_;
    unsigned		                       thread_id_;
    boost::atomic<int>		               working_num_;
    thread_buffer                          thread_buffer_;
    Callback                               callback_;
};
#endif