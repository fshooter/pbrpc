/*!
* @file log.h
* @brief 日志接口文件
*/
#ifndef _LOG_H__
#define _LOG_H__

namespace pbrpc {
namespace common {

/// 日志级别
enum LogLevel {
    LOG_LEVEL_VERBOSE = 1,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

static const int MAX_BUF_SIZE = 1023;

/// @brief 记录日志
///
/// 正常情况下，请不要直接使用本函数，使用 TraceXXX 宏
/// @see TraceVerbose TraceDebug TraceInfo TraceWarning TraceError
void Log(LogLevel level, const char* fmt, ...);

/// @brief 发送不可修复的关键性错误
///
/// 本函数将中止程序执行
void Fetal(const char* fmt, ...);

#ifdef TCL_ENABLE_LOG
#define Trace(Level, Format, ...) ::pbrpc::common::Log(Level, Format, __VA_ARGS__)
#else
#define Trace(Level, Format, ...)
#endif

#define TraceVerbose(Format, ...) Trace(::pbrpc::common::LogLevel::LOG_LEVEL_VERBOSE, Format, __VA_ARGS__)
#define TraceDebug(Format, ...) Trace(::pbrpc::common::LogLevel::LOG_LEVEL_DEBUG, Format, __VA_ARGS__)
#define TraceInfo(Format, ...) Trace(::pbrpc::common::LogLevel::LOG_LEVEL_INFO, Format, __VA_ARGS__)
#define TraceWarning(Format, ...) Trace(::pbrpc::common::LogLevel::LOG_LEVEL_WARNING, Format, __VA_ARGS__)
#define TraceWarning(Format, ...) Trace(::pbrpc::common::LogLevel::LOG_LEVEL_WARNING, Format, __VA_ARGS__)
#define TraceError(Format, ...) Trace(::pbrpc::common::LogLevel::LOG_LEVEL_ERROR, Format, __VA_ARGS__)

}
}

#endif