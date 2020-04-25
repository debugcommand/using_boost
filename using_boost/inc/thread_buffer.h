#ifndef _THREAD_BUFFER_H_
#define _THREAD_BUFFER_H_
#include <string>
#include "circular_buffer.h"
#include "realbuffer.h"
//用于线程之间通讯,像网络协议一样格式化解析

class thread_buffer
{
public:
    thread_buffer(int buffer_size,int data_size):cbuffer_(buffer_size),curLenght_(data_size)
    {
        tempdata_ = new char[curLenght_];
    }
    ~thread_buffer() {
        delete[] tempdata_;
    }
    //append
    bool append(int num) {
        return append((char*)&num, sizeof(num));
    }
    bool append(__int64 num) {
        return append((char*)&num, sizeof(num));
    }
    bool append(const char* pData) {
        if (!pData) {
            return false;
        }
        size_t len = strlen(pData);
        return append(pData, len);
    }
    bool append(const char* pData, size_t length) {
        if (!pData) {
            return false;
        }
        if (curLenght_ < length){//如果临时buff比写入的小了，先分配好,扩充1倍，防止频繁扩容
            curLenght_ *= 2;
            tempdata_ = (char*)realloc(tempdata_, curLenght_);
        }
        return cbuffer_.write(pData, length) > 0;
    }
    bool append(const std::string& str) {
        return append(str.c_str(), str.size());
    }
    bool append(buffer_base* pBuffer) {
        //长度
        append(pBuffer->GetLenght());
        //所有数据
        return append((char*)pBuffer, pBuffer->GetLenght());
    }
    //get
    bool GetInt(int& value) {
        return cbuffer_.read((char*)(&value), sizeof(int)) > 0;
    }
    bool GetInt64(__int64& value) {        
        return cbuffer_.read((char*)(&value), sizeof(__int64)) > 0;
    }
    bool GetData(char* data, size_t length) {
        return cbuffer_.read(data, length) > 0;
    }
    char* GetData(size_t length) {
        if (curLenght_ < length) {//如果临时buff比写入的小了       
            curLenght_ = length;
            tempdata_ = (char*)realloc(tempdata_, curLenght_);
        }
        if (cbuffer_.read(tempdata_, length) > 0)
            return tempdata_;
        return nullptr;
    }
    buffer_base* GetBuffer() {
        int len = 0;
        if (!GetInt(len))
            return nullptr;
        if(GetData(len) == nullptr)
            return nullptr;
        return (buffer_base*)tempdata_;
    }
    //
    size_t capacity() const {
        return cbuffer_.capacity();
    }
    size_t size() const {
        return cbuffer_.size();
    }
    void clear(){
        memset(tempdata_,0, curLenght_);
    }
private:
    CircularBuffer cbuffer_;
    char*	       tempdata_;
    int            curLenght_;
};

#endif // ! _THREAD_BUFFER_H_

