#include "listhead.h"

namespace pbrpc {
namespace common {

void ListHeadInitailize(ListHead* head) {
    head->next = head->prev = head;
}

static void __ListHeadInsert(ListHead* newEntry, ListHead* prev, ListHead* next) {
    next->prev = newEntry;
    newEntry->next = next;
    newEntry->prev = prev;
    prev->next = newEntry;
}

void ListHeadInsert(ListHead* pos, ListHead* entry) {
    __ListHeadInsert(entry, pos, pos->next);
}

void ListHeadInsertTail(ListHead* pos, ListHead* entry) {
    __ListHeadInsert(entry, pos->prev, pos);
}

void __ListHeadRemove(ListHead* prev, ListHead* next) {
    next->prev = prev;
    prev->next = next;
}

void ListHeadRemove(ListHead* entry) {
    __ListHeadRemove(entry->prev, entry->next);
    entry->prev = entry->prev = entry;
}

bool ListHeadEmpty(ListHead* head) {
    return head->next == head && head->prev == head;
}

}
}