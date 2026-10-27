#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BLCKSZ 8192  /* Standard PostgreSQL 8KB block size */

typedef uint32_t BlockNumber;
typedef uint16_t LocationIndex;
typedef uint32_t TransactionId;
typedef uint64_t Lsn;        /* Log Sequence Number (64-bit) */
typedef uint32_t Oid;        /* Object Identifier (Catalog) */

#define InvalidBlockNumber       ((BlockNumber) 0xFFFFFFFF)
#define InvalidTransactionId     ((TransactionId) 0)
#define FirstNormalTransactionId ((TransactionId) 3)

/* ItemPointerData (ctid): Tuple physical location (BlockNo, OffsetIndex) */
typedef struct ItemPointerData {
    BlockNumber bi_hi;       /* Block number */
    uint16_t    bi_lo;       /* Offset index in page slotted array */
} ItemPointerData;

typedef ItemPointerData* ItemPointer;

#endif /* COMMON_TYPES_H */
