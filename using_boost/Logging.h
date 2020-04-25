#ifndef _LOGGING_H_
#define _LOGGING_H_
#include <string>
#include "./ActiveThread.h"
#include "boost/atomic/atomic.hpp"

using namespace std;

enum eLogType {
    LOG_INFO,        //��Ϣ
    LOG_DEBUG,       //debug
    LOG_WARNNING,     //����
    LOG_ERROR,         //����
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
    void    ConvertTime2String(string& str,bool year = false,bool filename= false); //�Ƿ�����
    int		CreateLogFile(eLogType elogtype,string workDir);
    void    CheckBufferTimeout();
private:
    string        appName_;
    ActiveThread* logThread_;
    char          appPath_[256];
    boost::atomic<FILE*>	files_[LOG_MAX];
    boost::atomic<PTIME>	preFlushTime[LOG_MAX];
    thread_buffer*          buffers_[LOG_MAX];
    atomic_int              buffer_count[LOG_MAX];
    int           dump2File_count;    //д���ļ��Ļ�������
    int           dump2File_timeout;  //д���ļ���ʱ��,ms
};
#define LOG(p,type, msg, ...) Log(p,type, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

extern "C" void Log(void* p,int log_type, const char* fun, int line, char* msg, ...);
#endif