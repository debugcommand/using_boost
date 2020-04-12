#include "timers.h"
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
#if 0

    else if (strcmp(platform, "-s") == 0) {//sever
        test_tcpserver* pTServer = new test_tcpserver();
        pTServer->RunServer(atoi(GetCmdParam(2)));
    }
    else if (strcmp(platform, "-r") == 0) {//redis
        test_redis* pTRedis = new test_redis();
        pTRedis->RunRedis(GetCmdParam(2), atoi(GetCmdParam(3)));
    }
#endif // 0
    return 0;
}
