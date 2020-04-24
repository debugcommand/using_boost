#include "timers.h"
#include <ctime>
#include <iomanip>

const int span_time = 5000; //º‰∏Ù ±º‰ ms
static boost::posix_time::ptime sInitTime(boost::posix_time::microsec_clock::universal_time());

__int64 now_time(eTType _type) {
    switch (_type)
    {
    case eTType::eTType_sdy:
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    break;
    case eTType::eTType_sys:
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    break;
    case eTType::eTType_hr:
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    break;
    case eTType::eTType_dline:
    {
        auto time = boost::posix_time::microsec_clock::universal_time();
        return (time - sInitTime).total_milliseconds();
    }
    break;
    }
    return 0;
}

void show_current_time(const char* _str, eTType _type)
{
    /*auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << what << std::put_time(std::localtime(&time), "%F %T") << "\n";*/
    switch (_type)
    {
    case eTType::eTType_sdy:
    {
        std::cout << _str << ":" << now_time(_type) << "\n";
    }
    break;
    case eTType::eTType_sys:
    {
        std::cout << _str << ":"<< now_time(_type) << "\n";
    }
    break;
    case eTType::eTType_hr:
    {
        std::cout << _str << ":" << now_time(_type) << "\n";
    }
    break;
    case eTType::eTType_dline:
    {
        std::cout << _str << ":" << now_time(_type) << "\n";
    }
    break;
    }
}

timers::timers()
{
    mapActiveTimers.clear();
    activeTimer_idx_ = 0;
}

timers::~timers()
{
    for (auto itr = mapActiveTimers.begin(); itr != mapActiveTimers.end(); itr++)
    {
        delete itr->second;
    }
    mapActiveTimers.clear();
}

timers* timers::create() {
    return new timers();
}

void timers::stop() {
    delete this;
}

bool timers::addtimer(HeartbeatCallback pHeartbeatCallback, __int64 interval, eTType type, bool ifpersist, bool forcerun)
{
    ActiveTimer* at_ = new ActiveTimer();
    at_->interval_ = interval;
    at_->ttype_    = type;
    at_->hb_cb_    = pHeartbeatCallback;
    at_->persist_  = ifpersist;
    at_->index_ = activeTimer_idx_++;
    at_->timers_ = this;
    switch (type)
    {
    case eTType::eTType_sdy:
    {
        at_->timer_.sdy_timer_ = new boost::asio::steady_timer(io_);
    }
    break;
    case eTType::eTType_sys:
    {
        at_->timer_.sys_timer_ = new boost::asio::system_timer(io_);
    }
    break;
    case eTType::eTType_hr:
    {
        at_->timer_.hr_timer_ = new boost::asio::high_resolution_timer(io_);
    }
    break;
    case eTType::eTType_dline:
    {
        at_->timer_.deadline_timer_ = new boost::asio::deadline_timer(io_);
    }
    break;
    default:
        std::cout << "error! \n" << std::endl;
        return false;
    }
    mapActiveTimers[at_->index_] = at_;
    if (forcerun)
    {
        runtimer(at_,now_time(at_->ttype_));
    }
    return true;
}

void timers::removetimer(int index)
{
    mapActiveTimers.erase(index);
}

void timers::ActiveTimer::timeout()
{
    __int64 _now = now_time(ttype_);
    __int64 span = _now - start_;

    hb_cb_(_now, span);

    if (persist_)
    {
        running_ = false;
        timers_->runtimer(this, _now);
    }
    else
    {
        if (close_cb_) {
            close_cb_();
        }
        timers_->removetimer(index_);
    }
}

void timers::runtimer(ActiveTimer* pATimer, __int64 now_time_)
{
    if (pATimer->running_){
        return;
    }
    switch (pATimer->ttype_)
    {
    case eTType::eTType_sdy:
    {
        pATimer->timer_.sdy_timer_->expires_from_now(std::chrono::milliseconds(pATimer->interval_));
        pATimer->timer_.sdy_timer_->async_wait(std::bind(&timers::ActiveTimer::timeout, pATimer));
    }
    break;
    case eTType::eTType_sys:
    {
        pATimer->timer_.sys_timer_->expires_from_now(std::chrono::milliseconds(pATimer->interval_));
        pATimer->timer_.sys_timer_->async_wait(std::bind(&timers::ActiveTimer::timeout, pATimer));
    }
    break;
    case eTType::eTType_hr:
    {
        pATimer->timer_.hr_timer_->expires_from_now(std::chrono::milliseconds(pATimer->interval_));
        pATimer->timer_.hr_timer_->async_wait(std::bind(&timers::ActiveTimer::timeout, pATimer));
    }
    break;
    case eTType::eTType_dline:
    {
        pATimer->timer_.deadline_timer_->expires_from_now(boost::posix_time::milliseconds(pATimer->interval_));
        pATimer->timer_.deadline_timer_->async_wait(std::bind(&timers::ActiveTimer::timeout, pATimer));
    }
    break;
    }
    pATimer->start_ = now_time_;
    pATimer->running_ = true;
}

void timers::timeout()
{
    std::cout << "timeout" << std::endl;
}

void timers::tout(__int64 nTime, __int64 sTime) {
    std::cout << "timeout:" << nTime<<"-"<< sTime<< std::endl;
}


void timers::start()
{
    for (auto itr=mapActiveTimers.begin();itr != mapActiveTimers.end();++itr)
    {
        runtimer((itr->second), now_time(itr->second->ttype_));
    }
}

void timers::run() {
    io_.run();
}

void timers::test_run(timers* pT) {
    pT->addtimer(std::bind(&timers::tout, pT, std::placeholders::_1, std::placeholders::_2), 3000, eTType_dline, true,true);
    while (1)
    {
        pT->run();
        Sleep(1);
    }
}