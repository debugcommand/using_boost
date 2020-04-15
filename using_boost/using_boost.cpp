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
        timers* pTimer = timers::create();
        pTimer->test_run();
        /*show_current_time("system:",eTType_sys);
        Sleep(1000);
        show_current_time("system:", eTType_sys);
        Sleep(1000);
        show_current_time("steady:",eTType_sdy);
        Sleep(1000);
        show_current_time("steady:", eTType_sdy);
        Sleep(1000);
        show_current_time("hr:",eTType_hr);
        Sleep(1000);
        show_current_time("hr:", eTType_hr);
        Sleep(1000);
        show_current_time("dl:", eTType_dline);
        Sleep(1000);
        show_current_time("dl:", eTType_dline);       */ 
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