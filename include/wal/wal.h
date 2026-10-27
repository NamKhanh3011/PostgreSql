#ifndef WAL_H
#define WAL_H

#include "common/types.h"

typedef enum WalRecordType {
    WAL_INSERT_TUPLE = 1,
    WAL_DELETE_TUPLE,
    WAL_UPDATE_TUPLE,
    WAL_COMMIT,
    WAL_ABORT,
    WAL_CHECKPOINT
} WalRecordType;

typedef struct WalRecordHeader {
    Lsn             total_length;
    TransactionId   txid;
    WalRecordType   record_type;
    Lsn             prev_lsn;
    uint32_t        crc32;
} WalRecordHeader;

Lsn WalAppendRecord(WalRecordType type, TransactionId txid, const void* data, size_t size);
void WalFlush(Lsn target_lsn);
void WalRunRecovery(void);  /* ARIES: Analysis -> Redo -> Undo */

#endif /* WAL_H */
