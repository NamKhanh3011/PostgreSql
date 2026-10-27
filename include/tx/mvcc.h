#ifndef TX_MVCC_H
#define TX_MVCC_H

#include "common/types.h"
#include "storage/page.h"

#define MAX_ACTIVE_TXNS 128

typedef struct SnapshotData {
    TransactionId xmin;                      /* Oldest running transaction */
    TransactionId xmax;                      /* Highest completed TxID + 1 */
    size_t        active_count;
    TransactionId active_txns[MAX_ACTIVE_TXNS];
} SnapshotData;

typedef SnapshotData* Snapshot;

TransactionId TxManagerBegin(void);
void TxManagerCommit(TransactionId txid);
void TxManagerAbort(TransactionId txid);
Snapshot TxManagerGetSnapshot(void);
bool MVCCIsTupleVisible(HeapTupleHeaderData* tuple_header, Snapshot snapshot);

#endif /* TX_MVCC_H */
