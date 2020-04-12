#ifndef _TIMERS_H_
#define  _TIMERS_H_
/*boost asio��ʱ����2����
*1.deadline_timer
*2.waitable_timer��high_resolution_timer��steady_timer��system_timer
*����ʱ�Ӳ�ͬ����Ķ���ͬ��deadline_timer���õ�ʱ��Ϊboost::posix_time::ptime����һ��UTCʱ��
*system_timer - ϵͳ�ṩ��ʵʱʱ��-������Ҫ�õ�����ʱ��ĳ���
*high_resolution_timer - ��ǰϵͳʱ��������̵�ʱ��,û�������steady_timer������ʲô���
*steady_timer - ���ᱻ�����ĵ���ʱ��-������Ҫ�õ�ʱ�������������ʱ����������Ϊ�޸�ϵͳʱ�����Ӱ��ĳ���
*/
#include <sdkddkver.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp >
#include <boost/asio/steady_timer.hpp >
#include <boost/asio/system_timer.hpp >

const float timer_version = 1.0f;//��Ǽ�ʱ���İ汾

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