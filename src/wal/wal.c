#include "wal/wal.h"
#include <stdlib.h>

WALManager* WALOpen(const char* wal_path) {
    WALManager* wm = (WALManager*)malloc(sizeof(WALManager));
    if (!wm) return NULL;

    wm->wal_file = fopen(wal_path, "a+b");
    if (!wm->wal_file) {
        free(wm);
        return NULL;
    }

    fseek(wm->wal_file, 0, SEEK_END);
    wm->current_lsn = (XLogRecPtr)ftell(wm->wal_file);
    return wm;
}

XLogRecPtr WALWriteRecord(WALManager* wm, uint32_t xid, WALRecordType type, BlockNumber blk, LocationIndex slot, const char* data, uint32_t len) {
    if (!wm || !wm->wal_file) return 0;

    WALRecordHeader hdr;
    hdr.prev_lsn = wm->current_lsn;
    hdr.xid = xid;
    hdr.type = type;
    hdr.block_num = blk;
    hdr.slot_num = slot;
    hdr.data_len = len;

    fwrite(&hdr, sizeof(WALRecordHeader), 1, wm->wal_file);
    if (len > 0 && data != NULL) {
        fwrite(data, 1, len, wm->wal_file);
    }
    fflush(wm->wal_file);

    wm->current_lsn += sizeof(WALRecordHeader) + len;
    return wm->current_lsn;
}

void WALFlush(WALManager* wm) {
    if (wm && wm->wal_file) {
        fflush(wm->wal_file);
    }
}

void WALClose(WALManager* wm) {
    if (wm) {
        if (wm->wal_file) fclose(wm->wal_file);
        free(wm);
    }
}
