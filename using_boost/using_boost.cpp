// using_boost.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#if defined(WIN32)
#include <windows.h>
#endif

int main(int argc, char** argv)
{
#ifdef DEBUG
    system("pause");
#endif // DEBUG
    //commandline///////////////////
    auto GetCmdParam = [&](int idx) {
        return idx < argc ? argv[idx] : NULL;
    };
    const char* platform = GetCmdParam(1);
#if 0
    if (strcmp(platform, "-c") == 0)
    {//cleint
        test_tcpclient* pTClient = new test_tcpclient();
        pTClient->RunClient(GetCmdParam(2), atoi(GetCmdParam(3)), atoi(GetCmdParam(4)));
    }
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
