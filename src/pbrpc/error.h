/*!
 * @file error.h
 * @brief 错误代码定义
 */
#ifndef _PBRPC_ERROR_H__
#define _PBRPC_ERROR_H__

namespace pbrpc {

/// 错误代码
enum Error {
    E_OK = 0, //!< 正常
    E_INVALID_PARAMETER, //!< 无效参数
    E_INVALID_STATUS, //!< 无效状态
    E_WIN32_ERR //!< Win32错误
};

}

#endif // _PBRPC_ERROR_H__