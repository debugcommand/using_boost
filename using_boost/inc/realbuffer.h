#ifndef _REAL_BUFFER_H_
#define _REAL_BUFFER_H_
struct buffer_header
{
    int lenght;
    int type;
};
struct buffer_base
{
    buffer_header header;
    int GetLenght() { return header.lenght; }
    int GetType() { return header.type; }
};

struct testing :public buffer_base {
    testing() {
        header.lenght = sizeof(testing);
        header.type = 1;
    }
    int loop;
    char name[32];
};

struct logbuffer :public buffer_base {
    logbuffer() {
        header.lenght = sizeof(testing) - 2*sizeof(int);
        header.type = 2;
        memset(log_buffer, 0, 1024);
        log_type = 0;
        buffer_len = 0;
    }
    bool add_buffer(char* pbuffer,int length) {
        if (1024 - buffer_len < length)
        {
            return false;
        }
        memcpy(log_buffer + buffer_len, pbuffer, length);
        buffer_len += length;
        header.lenght += length;
        return true;
    }
    int  log_type;
    int  buffer_len;
    char log_buffer[1024];
};
#endif
