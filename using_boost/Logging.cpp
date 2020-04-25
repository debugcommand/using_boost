#include "logging.h"
#ifdef WIN32
#include <windows.h>
#endif // WIN32
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

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
    ::GetModuleFileNameA(0, appPath_, 256);
    dump2File_count = 10;
    dump2File_timeout = 1000;
}

Logging::~Logging()
{
    for (INT32 i = 0; i < LOG_MAX; ++i) {
        if (nullptr != files_[i]) {
            fclose(files_[i]);
        }
        if (nullptr != buffers_[i])
        {
            delete buffers_[i];
        }
    }

    delete logThread_;
}

namespace bfsys = boost::filesystem;
void   Logging::InitLoggin()
{
    PTIME nowTime = boost::posix_time::microsec_clock::local_time();
    string curWorkStr = appPath_;
    curWorkStr = curWorkStr.substr(0, curWorkStr.rfind("\\")) + "\\Log";
    if (!bfsys::exists(curWorkStr)) {
        boost::filesystem::create_directory(curWorkStr);
    }
    int n32Res = 0;
    for (int i = LOG_INFO; i < LOG_MAX; ++i) {
        files_[i] = NULL;
        buffer_count[i] = 0;
        preFlushTime[i] = nowTime;
        n32Res = CreateLogFile((eLogType)i, curWorkStr);
        if (n32Res != 0) {
            printf("Createfile(%d) failed", i);
        }

        buffers_[i] = new thread_buffer(1024*10,1024);
    }
    //
    logThread_ = ActiveThread::Create(std::bind(&Logging::BufferHandler,this,std::placeholders::_1));
    logThread_->AddTimer(std::bind(&Logging::CheckBufferTimeout, this), dump2File_timeout, eTType_dline, true);
    logThread_->Start();
}

int		Logging::CreateLogFile(eLogType elogtype, string workDir)
{
    string strPre;
    switch (elogtype) {
    case LOG_DEBUG:
        strPre = "DEBUG";
        break;
    case LOG_INFO:
        strPre = "INFO";
        break;
    case LOG_WARNNING:
        strPre = "WARNNING";
        break;
    case LOG_ERROR:
        strPre = "ERROR";
        break;
    default:
        return -1;
    }
    char str[128];
    int fd = -1;
    bool ifReuseCurDay = true; //是否复用本日log文件,会覆盖
    if (!ifReuseCurDay) {
        std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()).c_str();
        ConvertTime2String(strTime, true,true);
        _snprintf(str, sizeof(str), "%s/%s-%s-%s.log", workDir.c_str(), appName_.c_str(), strPre.c_str(), strTime.c_str());
        fd = open(str, O_WRONLY | O_CREAT | O_EXCL, 0664);        
    }
    else {
        std::string strTime = boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day());
        ConvertTime2String(strTime, true,true);
        _snprintf(str, sizeof(str), "%s/%s-%s-%s.log", workDir.c_str(), appName_.c_str(), strPre.c_str(), strTime.c_str());
        fd = open(str, O_WRONLY | O_CREAT, _S_IREAD | _S_IWRITE | _S_IEXEC);
    }
    if (fd == -1) {
        DWORD errorMessageID = ::GetLastError();
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        std::string message(messageBuffer, size);
        //Free the buffer.
        LocalFree(messageBuffer);

        printf("%s create(%d) file error %s\n", __FUNCTION__, (int)elogtype, message.c_str());
        return -1;
    }

    files_[elogtype] = fdopen(fd, "a");
    if (NULL == files_[elogtype]) {
        printf("%s open file(%d) failed\n", __FUNCTION__, (int)elogtype);
        return -1;
    }

    return 0;
}

void    Logging::CheckBufferTimeout()
{
    FILE* pCurFile = nullptr;
    for (size_t i = LOG_INFO; i < LOG_MAX; i++)
    {
        pCurFile = files_[i];
        if (pCurFile == nullptr) {
            continue;
        }
        thread_buffer* pDumpBuffer = buffers_[i];
        if (pDumpBuffer->size() <= 0) {
            continue;
        }
        fwrite(pDumpBuffer->GetData(pDumpBuffer->size()), 1, pDumpBuffer->size(), pCurFile);
        fflush(pCurFile);
        buffer_count[i] = 0;
        preFlushTime[i] = boost::posix_time::microsec_clock::local_time();
    }
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
    //
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
    //wirte log file
    {
        FILE* pCurFile = files_[log_buffer->log_type];
        if (pCurFile == nullptr) {
            return;
        }

        thread_buffer* pDumpBuffer = buffers_[log_buffer->log_type];
        bool ifDump2File = pDumpBuffer->capacity() >= log_buffer->buffer_len;
        if (ifDump2File && buffer_count[log_buffer->log_type] < dump2File_count) {
            pDumpBuffer->append(log_buffer->log_buffer, log_buffer->buffer_len);
            buffer_count[log_buffer->log_type]++;
        }
        else {
            if (pDumpBuffer->size() > 0) {
                fwrite(pDumpBuffer->GetData(pDumpBuffer->size()), 1, pDumpBuffer->size(), pCurFile);
            }

            fwrite(log_buffer->log_buffer, 1, log_buffer->buffer_len, pCurFile);
            fflush(pCurFile);
            buffer_count[log_buffer->log_type] = 0;
            preFlushTime[log_buffer->log_type] = boost::posix_time::microsec_clock::local_time();
        }
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

static string split_1 = string("-");
static string split_2 = string(":");
void Logging::ConvertTime2String(std::string& str, bool year, bool filename)
{    
    int pos = str.find('T');
    if (pos < 0)
    {
        return;
    }
    if (!year) {
        str.erase(0, 4);
        pos -= 4;
    }
    else{
        str.replace(4, 1, split_1);
    }
    str.replace(pos, 1, split_1);
    if (filename) {
        str.replace(pos + 3, 0, split_1);
        str.replace(pos + 6, 0, split_1);
    }
    else {
        str.replace(pos + 3, 0, split_2);
        str.replace(pos + 6, 0, split_2);
    }    
    str.replace(pos - 2, 0, split_1);
}
