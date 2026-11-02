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

ExecutorNode* CreateSeqScanExecutor(BufferPoolManager* bpm, BlockNumber total_blocks);
ExecutorNode* CreateFilterExecutor(ExecutorNode* child, bool (*predicate)(Tuple*));
ExecutorNode* CreateLimitExecutor(ExecutorNode* child, size_t limit);

#endif /* EXECUTOR_H */
