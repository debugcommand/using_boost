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
#include <sdkddkver.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp >
#include <boost/asio/steady_timer.hpp >
#include <boost/asio/system_timer.hpp >

const float timer_version = 1.0f;//标记计时器的版本

enum eTType
{
    eTType_sdy = 0,
    eTType_sys,
    eTType_hr,
    eTType_dline,
};

typedef std::function<void()> timer_cb;
class timers
{
public:
    timers(eTType type = eTType::eTType_sdy);
    ~timers();

    static timers* create(eTType _type = eTType::eTType_sdy);
    void setcb(timer_cb _cb);
    bool addtimer();
    void stop();
    void timeout();
    void run(bool _mult, bool async);
    void runtimer();
private:
    boost::asio::io_service io_;
    boost::asio::steady_timer* sdy_timer_;
    boost::asio::system_timer* sys_timer_;
    boost::asio::high_resolution_timer* hr_timer_;
    boost::asio::deadline_timer* deadline_timer_;
    int count_;
    eTType ttype_;
    bool async_;
};


#endif // !_TIMERS_H_