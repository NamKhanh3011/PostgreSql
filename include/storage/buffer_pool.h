#ifndef STORAGE_BUFFER_POOL_H
#define STORAGE_BUFFER_POOL_H

#include "common/types.h"
#include "storage/page.h"

#define BUFFER_POOL_SIZE 64  /* Default frame count */

typedef int32_t FrameId;

typedef struct BufferDesc {
    BlockNumber block_num;
    FrameId     frame_id;
    int32_t     pin_count;
    bool        is_dirty;
    bool        usage_flag;  /* Used by Clock Sweep replacer */
} BufferDesc;

typedef struct BufferPoolManager {
    char*       frames;      /* Contiguous memory: BUFFER_POOL_SIZE * BLCKSZ */
    BufferDesc  descriptors[BUFFER_POOL_SIZE];
    size_t      clock_hand;
    int         disk_fd;
} BufferPoolManager;

BufferPoolManager* BufferPoolCreate(const char* db_file_path);
Page BufferPoolFetchPage(BufferPoolManager* bpm, BlockNumber block_num);
void BufferPoolUnpinPage(BufferPoolManager* bpm, BlockNumber block_num, bool is_dirty);
void BufferPoolFlushAll(BufferPoolManager* bpm);
void BufferPoolDestroy(BufferPoolManager* bpm);

#endif /* STORAGE_BUFFER_POOL_H */
