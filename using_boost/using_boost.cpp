#include "timers.h"
#include "LogicActor.h"
#if defined(WIN32)
#include <windows.h>
#endif

int main(int argc, char** argv)
{
#ifdef _DEBUG
    system("pause");
#endif // DEBUG
    //commandline///////////////////
    auto GetCmdParam = [&](int idx) {
        return idx < argc ? argv[idx] : NULL;
    };
    const char* platform = GetCmdParam(1);
    if (strcmp(platform, "-t") == 0)
    {//timers
        timers* pTimer = timers::create((eTType)atoi(GetCmdParam(2)));
        pTimer->run(atoi(GetCmdParam(3)), atoi(GetCmdParam(4)));
    }
    else if (strcmp(platform, "-at") == 0)
    {
        LogicActor* pActor = new LogicActor();
        pActor->Start();
        for (; ;)
        {
            Sleep(1);
        }
    }
    return 0;
}
