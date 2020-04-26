#include "LogicActor.h"

LogicActor::LogicActor()
{
    thread_ = ActiveThread::Create(std::bind(&LogicActor::Handler, this, std::placeholders::_1));
}

LogicActor::~LogicActor()
{
    delete thread_;
}

void LogicActor::Handler(buffer_base*& pBuffer)
{
    if (!pBuffer)
    {
        std::cout << "LogicActor::handler" << std::endl;
        return;
    }
    switch (pBuffer->GetType()) {
    case 0:
        break;
    default:
        break;
    }
}

void LogicActor::Start() {

    thread_->AddTimer(std::bind(&LogicActor::hb_sys,this, std::placeholders::_1, std::placeholders::_2),2000,eTType_sys,true);
    thread_->AddTimer(std::bind(&LogicActor::hb_hr, this, std::placeholders::_1, std::placeholders::_2), 2000, eTType_hr, true);  
    thread_->AddTimer(std::bind(&LogicActor::hb_sdy, this, std::placeholders::_1, std::placeholders::_2), 2000, eTType_sdy, true);
    thread_->AddTimer(std::bind(&LogicActor::hb_dl, this, std::placeholders::_1, std::placeholders::_2), 3000, eTType_dline, true);

    thread_->Start();
}

void LogicActor::Send2LogicThread(const char* pData, int n32Length) {
    thread_->Send(pData, n32Length);
}

void LogicActor::Send2LogicThread(buffer_base* pBuffer) {
    thread_->Send(pBuffer);
}

int sys_call_time = 3;
void LogicActor::hb_sys(__int64 curTime, __int64 spanTime)
{
    if (sys_call_time > 0)
    {
        std::cout << "sys:" << curTime << "-" << spanTime  << "\n";
        sys_call_time--;
    }
}

int hr_call_time = 3;
void LogicActor::hb_hr(__int64 curTime, __int64 spanTime) {
    if (hr_call_time > 0)
    {
        std::cout << "hr:" << curTime << "-" << spanTime << "\n";
        hr_call_time--;
    }
}

int sdy_call_time = 3;
void LogicActor::hb_sdy(__int64 curTime, __int64 spanTime) {
    if (sdy_call_time > 0) {
        sdy_call_time--;
        std::cout << "sdy:" << curTime << "-" << spanTime << "\n";
    }    
}

int dl_call_time = 2;
void LogicActor::hb_dl(__int64 curTime, __int64 spanTime) {
    if (dl_call_time > 0) {
        dl_call_time--;
        std::cout << "dl:" << curTime << "-" << spanTime << "\n";
    }
}