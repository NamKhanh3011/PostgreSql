#ifndef STORAGE_PAGE_H
#define STORAGE_PAGE_H

#include "common/types.h"

/* LinePointer (ItemIdData): 32-bit entry in slotted page array */
typedef struct ItemIdData {
    unsigned lp_off:15;      /* Byte offset to tuple data */
    unsigned lp_flags:2;     /* State flags: UNUSED, USED, DEAD */
    unsigned lp_len:15;      /* Byte length of tuple */
} ItemIdData;

typedef ItemIdData* ItemId;

#define LP_UNUSED   0
#define LP_USED     1
#define LP_DEAD     2

/* Slotted Page Header: Fixed at the beginning of each 8KB block */
typedef struct PageHeaderData {
    Lsn             pd_lsn;      /* LSN of last modification (WAL) */
    uint16_t        pd_checksum; /* Page checksum */
    uint16_t        pd_flags;    /* Page status flags */
    LocationIndex   pd_lower;    /* Offset to start of free space (after line pointers) */
    LocationIndex   pd_upper;    /* Offset to end of free space (before tuple data) */
    LocationIndex   pd_special;  /* Offset to special space (used by B-Tree) */
    uint16_t        pd_pagesize_version;
    ItemIdData      pd_linp[1];  /* Start of line pointer array */
} PageHeaderData;

typedef char* Page;

/* Tuple Header: PostgreSQL-style MVCC metadata attached to every row */
typedef struct HeapTupleHeaderData {
    TransactionId   t_xmin;      /* Inserting Transaction ID */
    TransactionId   t_xmax;      /* Deleting or updating Transaction ID */
    ItemPointerData t_ctid;      /* Current or newer physical Tuple ID */
    uint16_t        t_infomask;  /* Visibility status bit flags */
    uint8_t         t_hoff;      /* Header offset to user payload */
} HeapTupleHeaderData;

void PageInit(Page page, size_t pageSize);
bool PageAddItem(Page page, const char* item, size_t size, LocationIndex* outIndex);
char* PageGetItem(Page page, LocationIndex index, size_t* outSize);

#endif /* STORAGE_PAGE_H */
