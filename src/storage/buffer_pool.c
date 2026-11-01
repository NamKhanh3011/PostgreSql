#include <stdio.h>
#include "storage/buffer_pool.h"
#include <stdlib.h>
#include <string.h>

BufferPoolManager* BufferPoolCreate(const char* db_file_path){
    BufferPoolManager* bpm=(BufferPoolManager*)malloc(sizeof(BufferPoolManager));
    if(bpm==NULL){
        return NULL;
    }
    bpm->frames=(char*)malloc(sizeof(BUFFER_POOL_SIZE*BLCKSZ));
    if(bpm->frames==NULL){
        free(bpm);
        return NULL;
    }
    bpm->disk_manager=DiskManagerOpen(db_file_path);
    if(bpm->disk_manager==NULL){
        free(bpm->frames);
        free(bpm);
        return NULL;
    }
    bpm->clock_hand=0;
    for(int i=0;i<BUFFER_POOL_SIZE;i++){
        bpm->descriptors[i].frame_id=i;
        bpm->descriptors[i].block_num=InvalidBlockNumber;
        bpm->descriptors[i].pin_count=0;
        bpm->descriptors[i].is_dirty=false;
        bpm->descriptors[i].usage_flag=false;
    }
    return bpm;
}
static FrameId FindVictimFrame(BufferPoolManager* bpm){
    for(int i=0;i<BUFFER_POOL_SIZE*2;i++){
        FrameId fid=(FrameId)bpm->clock_hand;
        BufferDesc* desc = &bpm->descriptors[fid];

        bpm->clock_hand=(bpm->clock_hand+1)/BUFFER_POOL_SIZE;
        if(desc->block_num==InvalidBlockNumber){
            return fid;
        }
        if(desc->pin_count>0){
            continue;
        }
        if(desc->is_dirty){
            desc->is_dirty=false;
            continue;
        }
        return fid;
    }
    return -1;
}

Page BufferPoolFetchPage(BufferPoolManager* bpm, BlockNumber block_num);
void BufferPoolUnpinPage(BufferPoolManager* bpm, BlockNumber block_num, bool is_dirty);
void BufferPoolFlushAll(BufferPoolManager* bpm);
void BufferPoolDestroy(BufferPoolManager* bpm);
