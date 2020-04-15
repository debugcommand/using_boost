#ifndef _LOGIC_ACTOR_H_
#define _LOGIC_ACTOR_H_
#include "./ActiveThread.h"

class LogicActor
{
public:
    LogicActor();
    ~LogicActor();

    void			Start();
    void            Handler(buffer_base*& pBuffer);
    void            Send2LogicThread(const char* pData, int n32Length);
    void            Send2LogicThread(buffer_base* pBuffer);

    void hb_sys(__int64 curTime, __int64 spanTime);
    void hb_hr(__int64 curTime, __int64 spanTime);
    void hb_sdy(__int64 curTime, __int64 spanTime);
    void hb_dl(__int64 curTime, __int64 spanTime);
private:
    ActiveThread* thread_;
};
#endif // !_LOGIC_ACTOR_H_

