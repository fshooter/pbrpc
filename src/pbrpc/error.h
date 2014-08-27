#ifndef _PBRPC_ERROR_H__
#define _PBRPC_ERROR_H__

namespace pbrpc {

enum Error {
    E_OK = 0,
    E_INVALID_PARAMETER,
    E_INVALID_STATUS,
    E_WIN32_ERR
};

}

#endif // _PBRPC_ERROR_H__