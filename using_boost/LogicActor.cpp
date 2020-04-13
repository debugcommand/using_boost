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
    thread_->Start();
}

void LogicActor::Send2LogicThread(const char* pData, int n32Length) {
    thread_->Send(pData, n32Length);
}

void LogicActor::Send2LogicThread(buffer_base* pBuffer) {
    thread_->Send(pBuffer);
}