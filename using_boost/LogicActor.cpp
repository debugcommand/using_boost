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
    switch (pBuffer->GetType()) {
    case 0:
        break;
    default:
        break;
    }
}

void LogicActor::Start() {

    thread_->AddTimer(std::bind(&LogicActor::hb_sys,this, std::placeholders::_1, std::placeholders::_2),5000,eTType_sys,true);
    thread_->AddTimer(std::bind(&LogicActor::hb_hr, this, std::placeholders::_1, std::placeholders::_2), 4000, eTType_hr, true);  
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

void LogicActor::hb_sys(__int64 curTime, __int64 spanTime)
{
    std::cout <<"sys:" << curTime <<"-"<< spanTime <<"\n";
}

void LogicActor::hb_hr(__int64 curTime, __int64 spanTime) {
    std::cout << "hr:" << curTime << "-" << spanTime << "\n";
}

void LogicActor::hb_sdy(__int64 curTime, __int64 spanTime) {
    std::cout << "sdy:" << curTime << "-" << spanTime << "\n";
}

void LogicActor::hb_dl(__int64 curTime, __int64 spanTime) {
    std::cout << "dl:" << curTime << "-" << spanTime << "\n";
}