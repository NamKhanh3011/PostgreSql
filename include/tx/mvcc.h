#ifndef MVCC_H
#define MVCC_H

#include "common/types.h"
#include <stdbool.h>

typedef uint32_t TransactionId;
#define InvalidTransactionId 0
#define FirstNormalTransactionId 100

typedef struct HeapTupleHeaderData {
    TransactionId t_xmin; 
    TransactionId t_xmax; 
    uint32_t      t_cid; 
} HeapTupleHeaderData;

typedef struct SnapshotData {
    TransactionId xmin; 
    TransactionId xmax; 
} SnapshotData;
typedef SnapshotData* Snapshot;

bool HeapTupleSatisfiesVisibility(HeapTupleHeaderData* tuple_hdr, Snapshot snapshot);

#endif
