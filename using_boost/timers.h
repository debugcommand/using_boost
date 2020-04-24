#ifndef _TIMERS_H_
#define  _TIMERS_H_
/*boost asio定时器分2大类
*1.deadline_timer
*2.waitable_timer：high_resolution_timer、steady_timer、system_timer
*除了时钟不同，别的都相同，deadline_timer采用的时钟为boost::posix_time::ptime，是一种UTC时钟
*system_timer - 系统提供的实时时钟-用在需要得到绝对时点的场景
*high_resolution_timer - 当前系统时钟周期最短的时钟,没搞清楚与steady_timer到底有什么差别
*steady_timer - 不会被调整的单调时钟-用在需要得到时间间隔，并且这个时间间隔不会因为修改系统时间而受影响的场景
*/
#if defined(WIN32)
#include <sdkddkver.h>
#endif
#include <iostream>
#include <chrono>
#include <thread>
#include <map>
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp >
#include <boost/asio/steady_timer.hpp >
#include <boost/asio/system_timer.hpp >
#include <boost/date_time/posix_time/posix_time.hpp> 

const float timer_version = 2.0f;//标记计时器的版本
typedef std::function<void(__int64, __int64)> HeartbeatCallback;
typedef std::function<void()> CloseCallBack;
enum eTType
{
    eTType_sdy = 0,
    eTType_sys,
    eTType_hr,
    eTType_dline,
    eTType_max
};

class timers
{
private:
    class ActiveTimer
    {
    public:
        void timeout();
        ActiveTimer()
        {
            index_ = 0;
            ttype_ = eTType_max;
            hb_cb_ = nullptr;
            interval_ = 0;
            persist_ = false;
            timer_.sdy_timer_ = nullptr;
            close_cb_ = nullptr;
            timers_ = nullptr;
            running_ = false;
        }
        ~ActiveTimer()
        {
            clearTimer();
        }
        void clearTimer() {
            switch (ttype_)
            {
            case eTType_sdy:
            {
                if (timer_.sdy_timer_)
                {
                    delete timer_.sdy_timer_;
                    timer_.sdy_timer_ = nullptr;
                }
            }   
            return;
            case eTType_sys:
            {
                if (timer_.sys_timer_)
                {
                    delete timer_.sys_timer_;
                    timer_.sys_timer_ = nullptr;
                }
            }
            return;
            case eTType_hr:
            {
                if (timer_.hr_timer_)
                {
                    delete timer_.hr_timer_;
                    timer_.hr_timer_ = nullptr;
                }
            }
            return;
            case eTType_dline:
            {
                if (timer_.deadline_timer_)
                {
                    delete timer_.deadline_timer_;
                    timer_.deadline_timer_ = nullptr;
                }
            }
            return;
            default:
                return;
            }

        }
        union
        {
            boost::asio::steady_timer* sdy_timer_;
            boost::asio::system_timer* sys_timer_;
            boost::asio::high_resolution_timer* hr_timer_;
            boost::asio::deadline_timer* deadline_timer_;
        } timer_;
        int       index_;
        eTType    ttype_;
        HeartbeatCallback hb_cb_;
        CloseCallBack     close_cb_;
        __int64   interval_;
        __int64   start_;
        bool      persist_;
        timers*   timers_;
        bool      running_;
    };
public:
    timers();
    ~timers();

    static timers* create();
    bool addtimer(HeartbeatCallback pHeartbeatCallback, __int64 interval, eTType type,bool ifpersist,bool forcerun = false);
    void stop();    
    static void test_run(timers* pT);
    void runtimer(ActiveTimer* pATimer,__int64 now_time_);
    void removetimer(int index);
    void start();
    void run();
    void timeout();
    void tout(__int64 nTime, __int64 sTime);
private:
    boost::asio::io_service      io_;
    std::map<int,ActiveTimer*>   mapActiveTimers;
    int                          activeTimer_idx_;
};
extern void show_current_time(const char* _str, eTType _type);

#endif // !_TIMERS_H_