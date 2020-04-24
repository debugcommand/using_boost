#ifndef _LOGGING_H_
#define _LOGGING_H_
#include <string>
#include "./ActiveThread.h"

using namespace std;

enum eLogType {
    LOG_INFO,        //ÐÅÏ¢
    LOG_DEBUG,       //debug
    LOG_WARNING,     //¾¯¸æ
    LOG_ERROR,         //´íÎó
    LOG_MAX,
};

enum eLogColor {
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_PINK,
};

using PTIME = boost::posix_time::ptime;
class Logging
{
public:
    Logging();
    ~Logging();

    void   InitLoggin();
    void   Log(int log_type,const char* fun, int line, char* msg, ...);
    void   WriteMsg(eLogType elogtype, const char* fun, int line, char* msg);
private:    
    void    BufferHandler(buffer_base*& pBuffer);
    void    ConvertTime2String(string& str);
private:
    string        appName_;
    ActiveThread* logThread_;
};
#define LOG(p,type, msg, ...) Log(p,type, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

extern "C" void Log(void* p,int log_type, const char* fun, int line, char* msg, ...);
#endif