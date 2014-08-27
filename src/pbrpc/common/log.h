/*!
* @file log.h
* @brief ��־�ӿ��ļ�
*/
#ifndef _LOG_H__
#define _LOG_H__

namespace pbrpc {
namespace common {

/// ��־����
enum LogLevel {
    LOG_LEVEL_VERBOSE = 1,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

static const int MAX_BUF_SIZE = 1023;

/// @brief ��¼��־
///
/// ��������£��벻Ҫֱ��ʹ�ñ�������ʹ�� TraceXXX ��
/// @see TraceVerbose TraceDebug TraceInfo TraceWarning TraceError
void Log(LogLevel level, const char* fmt, ...);

/// @brief ���Ͳ����޸��Ĺؼ��Դ���
///
/// ����������ֹ����ִ��
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