#ifndef _PBRPC_SERVER_C_LIST_H__
#define _PBRPC_SERVER_C_LIST_H__
#include <pbrpc/config.h>

namespace pbrpc {
namespace common {

struct ListHead {
    ListHead *next, *prev;
};

void ListHeadInitailize(ListHead* head);

void ListHeadInsert(ListHead* pos, ListHead* entry);

void ListHeadInsertTail(ListHead* pos, ListHead* entry);

void ListHeadRemove(ListHead* entry);

bool ListHeadEmpty(ListHead* head);

#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member) ))

}
}

#endif // _PBRPC_SERVER_C_LIST_H__