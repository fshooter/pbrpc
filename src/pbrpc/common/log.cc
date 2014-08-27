/*!
* @file log.cpp
* @brief 日志实现文件
*/
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include "log.h"

namespace pbrpc {
namespace common {

void Log(LogLevel level, const char* fmt, ...) {
    char buf[MAX_BUF_SIZE + 1];
    va_list args;
    va_start(args, fmt);
    _vsnprintf_s(buf, MAX_BUF_SIZE, fmt, args);
    buf[MAX_BUF_SIZE] = 0;
    OutputDebugString(buf);
}

void Fetal(const char* fmt, ...) {
    char buf[MAX_BUF_SIZE + 1];
    va_list args;
    va_start(args, fmt);
    _vsnprintf_s(buf, MAX_BUF_SIZE, fmt, args);
    buf[MAX_BUF_SIZE] = 0;
    OutputDebugString(buf);
    exit(-1);
}

}
}