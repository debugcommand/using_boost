#include "timers.h"
#include <ctime>
#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp> 

const int span_time = 5000; //º‰∏Ù ±º‰ ms
static boost::posix_time::ptime sInitTime(boost::posix_time::microsec_clock::universal_time());
void show_current_time(const char* _str, eTType _type)
{
    /*auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << what << std::put_time(std::localtime(&time), "%F %T") << "\n";*/
    switch (_type)
    {
    case eTType::eTType_sdy:
    {
        auto time = std::chrono::steady_clock::now();
        std::cout << _str << ":" << time.time_since_epoch().count() << "\n";
    }
    break;
    case eTType::eTType_sys:
    {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cout << _str << ":"<< time << "\n";
    }
    break;
    case eTType::eTType_hr:
    {
        auto time = std::chrono::high_resolution_clock::now();
        std::cout << _str << ":" << time.time_since_epoch().count() << "\n";
    }
    break;
    case eTType::eTType_dline:
    {
        auto time = boost::posix_time::microsec_clock::universal_time();
        std::cout << _str << ":" << (time- sInitTime).total_milliseconds() << "\n";
    }
    break;
    }
}

timers::timers(eTType type) : count_(0), ttype_(type), async_(true)
{
    sdy_timer_ = nullptr;
    sys_timer_ = nullptr;
    hr_timer_ = nullptr;
    deadline_timer_ = nullptr;
}

timers::~timers()
{
    if (sdy_timer_)
    {
        delete sdy_timer_;
    }
    if (sys_timer_)
    {
        delete sys_timer_;
    }
    if (hr_timer_)
    {
        delete hr_timer_;
    }
    if (deadline_timer_) {
        delete deadline_timer_;
    }
}

timers* timers::create(eTType _type) {
    std::cout << "timer type:" << _type <<  std::endl;
    return new timers(_type);
}

void timers::setcb(timer_cb _cb)
{
}

void timers::stop() {
    delete this;
}

bool timers::addtimer()
{
    switch (ttype_)
    {
    case eTType::eTType_sdy:
    {        
        if (!sdy_timer_)
            sdy_timer_ = new boost::asio::steady_timer(io_);
        sdy_timer_->expires_from_now(std::chrono::milliseconds(span_time));
        show_current_time("begin wait:", ttype_);
        sdy_timer_->async_wait(std::bind(&timers::timeout, this));
    }
    break;
    case eTType::eTType_sys:
    {
        if (!sys_timer_)
            sys_timer_ = new boost::asio::system_timer(io_);
        sys_timer_->expires_from_now(std::chrono::milliseconds(span_time));
        show_current_time("begin wait:", ttype_);
        sys_timer_->async_wait(std::bind(&timers::timeout, this));
    }
    break;
    case eTType::eTType_hr:
    {
        if (!hr_timer_)
            hr_timer_ = new boost::asio::high_resolution_timer(io_);        
        hr_timer_->expires_from_now(std::chrono::milliseconds(span_time));
        show_current_time("begin wait:", ttype_);
        hr_timer_->async_wait(std::bind(&timers::timeout, this));
    }
    break;
    case eTType::eTType_dline:
    {
        if (!deadline_timer_)
            deadline_timer_ = new boost::asio::deadline_timer(io_);
        deadline_timer_->expires_from_now(boost::posix_time::seconds(span_time));
        show_current_time("begin wait:", ttype_);
        deadline_timer_->async_wait(std::bind(&timers::timeout, this));
    }
    break;
    default:
        std::cout << "Final begin!\n" << std::endl;
        return false;
    }
    return true;
}

void timers::timeout() {
    show_current_time("end   wait:", ttype_);
    if (count_ < 100) {
        if (async_)
        {
            addtimer();
        }
    }
    else
    {
        std::cout << "stop! \n";
        stop();
    }
}

void timers::runtimer()
{
    if (async_)
    {
        addtimer();
        io_.run();
    }
    else
    {
        switch (ttype_)
        {
        case eTType::eTType_sdy:
        {
            if (!sdy_timer_)
                sdy_timer_ = new boost::asio::steady_timer(io_);
            sdy_timer_->expires_from_now(std::chrono::milliseconds(span_time));
            show_current_time("begin wait:", ttype_);
            sdy_timer_->wait();
        }
        break;
        case eTType::eTType_sys:
        {
            if (!sys_timer_)
                sys_timer_ = new boost::asio::system_timer(io_);
            sys_timer_->expires_from_now(std::chrono::milliseconds(span_time));
            show_current_time("begin wait:", ttype_);
            sys_timer_->wait();            
        }
        break;
        case eTType::eTType_hr:
        {
            if (!hr_timer_)
                hr_timer_ = new boost::asio::high_resolution_timer(io_);
            hr_timer_->expires_from_now(std::chrono::milliseconds(span_time));
            show_current_time("begin wait:", ttype_);
            hr_timer_->wait();
        }
        break;
        case eTType::eTType_dline:
        {
            if (!deadline_timer_)
                deadline_timer_ = new boost::asio::deadline_timer(io_);
            deadline_timer_->expires_from_now(boost::posix_time::seconds(span_time));
            show_current_time("begin wait:", ttype_);
            deadline_timer_->wait();
        }
        break;
        }
        timeout();        
    }
}

void timers::run(bool async, bool mult_) {
    std::cout << "async_:" << async << "-mult_:" << mult_ << std::endl;
    async_ = async;
    if(mult_)
    {
        std::thread t;
        t = std::thread(std::mem_fn(&timers::runtimer), this);
        t.detach();
    }
    for (;;)
    {
        if (!mult_)
        {
            runtimer();
        }
        Sleep(1);
    }
}