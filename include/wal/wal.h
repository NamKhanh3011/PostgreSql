#ifndef WAL_H
#define WAL_H

#include "common/types.h"
#include <stdint.h>
#include <stdio.h>

typedef uint64_t XLogRecPtr; 

typedef enum {
    WAL_INSERT_TUPLE = 1,
    WAL_COMMIT       = 2,
    WAL_ABORT        = 3
} WALRecordType;

typedef struct WALRecordHeader {
    XLogRecPtr    prev_lsn;
    uint32_t      xid;
    WALRecordType type;
    BlockNumber   block_num;
    LocationIndex slot_num;
    uint32_t      data_len;
} WALRecordHeader;

typedef struct WALManager {
    FILE*      wal_file;
    XLogRecPtr current_lsn;
} WALManager;

WALManager* WALOpen(const char* wal_path);
XLogRecPtr  WALWriteRecord(WALManager* wm, uint32_t xid, WALRecordType type, BlockNumber blk, LocationIndex slot, const char* data, uint32_t len);
void        WALFlush(WALManager* wm);
void        WALClose(WALManager* wm);

#endif
