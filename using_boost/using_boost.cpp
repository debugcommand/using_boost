﻿#include "timers.h"
#include "LogicActor.h"
#include "smart_ptr_test.h"
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
        pTimer->test_run(pTimer);
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
    else if (strcmp(platform, "-spt") == 0) {
        auto_ptr_test3();
        for (; ;)
        {
            Sleep(1);
        }
    }
    return 0;
}