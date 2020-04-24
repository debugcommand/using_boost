#include "logging.h"
#ifdef WIN32
#include <windows.h>
#endif // WIN32

extern "C" void Log(void* p,int log_type, const char* fun, int line, char* msg, ...) {
    char tmp[1024] = { 0 };
    va_list pArg = NULL;
    va_start(pArg, msg);
    vsnprintf(tmp, sizeof(tmp), msg, pArg);
    va_end(pArg);
    ((Logging*)p)->WriteMsg((eLogType)log_type, fun, line, tmp);
}

static eLogColor eLogColorArr[LOG_MAX] = { COLOR_GREEN,COLOR_PINK,COLOR_YELLOW,COLOR_RED};
#ifdef WIN32
// Returns the character attribute for the given color.
static WORD GetColorAttribute(eLogColor color) {
    switch (color) {
    case COLOR_RED:    return FOREGROUND_RED;
    case COLOR_GREEN:  return FOREGROUND_GREEN;
    case COLOR_YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN;
    case COLOR_PINK:   return FOREGROUND_BLUE | FOREGROUND_RED;
    default:           return 0;
    }
}
#else
// Returns the ANSI color code for the given color.
static const char* GetAnsiColorCode(eLogColor color) {
    switch (color) {
    case COLOR_RED:     return "1";
    case COLOR_GREEN:   return "2";
    case COLOR_YELLOW:  return "3";
    case COLOR_DEFAULT:  return "";
    };
    return NULL; // stop warning about return type.
}
#endif

Logging::Logging()
{
    appName_ = string("using_boost");
}

Logging::~Logging()
{
}

void   Logging::InitLoggin()
{
    logThread_ = ActiveThread::Create(std::bind(&Logging::BufferHandler,this,std::placeholders::_1));
    logThread_->Start();
}

void Logging::BufferHandler(buffer_base*& pBuffer)
{
    if (pBuffer->GetType() != 2)
        return;
    logbuffer* log_buffer = (logbuffer*)pBuffer;
    PTIME nowTime = boost::posix_time::microsec_clock::local_time();
    string DateStr = boost::posix_time::to_iso_string(nowTime);
    ConvertTime2String(DateStr);
    memcpy(log_buffer->log_buffer, DateStr.c_str(), 19);
    {//print
        const eLogColor color = eLogColorArr[log_buffer->log_type];
#ifdef WIN32
        const HANDLE stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO buffer_info;
        GetConsoleScreenBufferInfo(stderr_handle, &buffer_info);
        const WORD old_color_attrs = buffer_info.wAttributes;
        fflush(stderr);
        SetConsoleTextAttribute(stderr_handle,GetColorAttribute(color) | FOREGROUND_INTENSITY);

        fwrite(log_buffer->log_buffer, 1, log_buffer->buffer_len, stderr);
        fflush(stderr);
        SetConsoleTextAttribute(stderr_handle, old_color_attrs);
#else
        fprintf(stderr, "\033[0;3%sm", GetAnsiColorCode(color));
        fwrite(message, len, 1, stderr);
        fprintf(stderr, "\033[m");  // Resets the terminal to default.
#endif
    }
}

void   Logging::Log(int log_type, const char* fun, int line, char* msg, ...)
{
    char tmp[1024] = { 0 };
    va_list pArg = NULL;
    va_start(pArg, msg);
    vsnprintf(tmp, sizeof(tmp), msg, pArg);
    va_end(pArg);
    WriteMsg((eLogType)log_type, fun, line, tmp);
}

void	Logging::WriteMsg(eLogType elogtype, const char* fun, int line, char* msg)
{
    char tmp[1024] = { 0 };
    logbuffer sLogBuffer;
    sLogBuffer.log_type = elogtype;
    //中间留白，方便插入时间，对齐整洁一些
    _snprintf(tmp, sizeof(tmp), "                                          %d %s:%d %s\n", GetCurrentThreadId(), fun, line, msg);
    sLogBuffer.add_buffer(tmp,strlen(tmp));
    logThread_->Send(&sLogBuffer);
}

void Logging::ConvertTime2String(std::string& str)
{
    str.erase(0, 4);           //去掉年
    int pos = str.find('T');
    str.replace(pos, 1, string("-"));
    str.replace(pos + 3, 0, string(":"));
    str.replace(pos + 6, 0, string(":"));
    str.replace(pos - 2, 0, string("-"));
}
