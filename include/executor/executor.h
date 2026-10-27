#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "common/types.h"
#include "storage/page.h"

typedef struct Tuple {
    HeapTupleHeaderData header;
    char*               data;
    size_t              size;
} Tuple;

/* Volcano Iterator Interface */
typedef struct ExecutorNode {
    void (*Init)(struct ExecutorNode* self);
    Tuple* (*Next)(struct ExecutorNode* self);
    void (*Close)(struct ExecutorNode* self);
    void* node_state;
} ExecutorNode;

ExecutorNode* CreateSeqScanExecutor(const char* table_name);
ExecutorNode* CreateFilterExecutor(ExecutorNode* child, bool (*predicate)(Tuple*));

#endif /* EXECUTOR_H */
