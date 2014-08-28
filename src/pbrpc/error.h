/*!
 * @file error.h
 * @brief ������붨��
 */
#ifndef _PBRPC_ERROR_H__
#define _PBRPC_ERROR_H__

namespace pbrpc {

/// �������
enum Error {
    E_OK = 0, //!< ����
    E_INVALID_PARAMETER, //!< ��Ч����
    E_INVALID_STATUS, //!< ��Ч״̬
    E_WIN32_ERR //!< Win32����
};

}

#endif // _PBRPC_ERROR_H__