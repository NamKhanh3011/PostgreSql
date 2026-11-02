#include <stdio.h>
#include "index/btree.h"
#include "storage/buffer_pool.h"
#include "storage/disk_manager.h"
#include <string.h>
void BTreeInit(BTreeIndex* idx, const char* name, BufferPoolManager* bpm){
    idx->root_block=InvalidBlockNumber;
    idx->index_name=name;
    idx->bpm=bpm;
}
static int BTreeBinarySearch(const int64_t keys[], int num_keys, int64_t target, bool* found){
    int low=0;
    int high=num_keys-1;
    *found=false;
    while(low<=high){
        int mid=(low+high)/2;
        if(key[mid]==target){
            *found=true;
            return mid;
        }
        else if(key[mid]<target){
            low=mid+1;
        }
        else{
            high=mid-1;
        }
    }
    return low;
}
bool BTreeFind(BTreeIndex* idx, int64_t key, ItemPointerData* outTid);
bool BTreeInsert(BTreeIndex* idx, int64_t key, ItemPointerData tid);
