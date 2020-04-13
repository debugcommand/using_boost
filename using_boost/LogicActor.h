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
private:
    ActiveThread* thread_;
};
#endif // !_LOGIC_ACTOR_H_

