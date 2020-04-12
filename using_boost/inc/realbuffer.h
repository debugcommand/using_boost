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

#endif
