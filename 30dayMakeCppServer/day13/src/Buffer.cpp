#include "Buffer.h"
#include <cstring>
#include <iostream>
#include <string>

Buffer::Buffer() {}
Buffer::~Buffer() {}
void Buffer::append(const char *str, int size)
{
    for(int i=0;i<size;i++){
        if(str[i]!='\0'){
            buf.push_back(str[i]);
        }
        else{
            break;
        }
    }
}

ssize_t Buffer::size()
{
    return buf.size();
}
const char *Buffer::c_str()
{
    return buf.c_str();
}
void Buffer::clear()
{
    buf.clear();
}
void Buffer::getline()
{
    buf.clear();
    std::getline(std::cin, buf);
}

void Buffer::setBuf(const char* buf){
    this->buf.clear();
    this->buf.append(buf);
}

void Buffer::setBuf(const std::string buf){
    this->buf.clear();
    this->buf.append(buf.c_str());
}