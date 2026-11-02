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
bool BTreeFind(BTreeIndex* idx, int64_t key, ItemPointerData* outTid){
    if(idx==NULL||idx->root_block==InvalidBlockNumber){
        return false;
    }
    BlockNumber curr_block=idx->root_block;
    bool found=false;
    while(curr_block!=InvalidBlockNumber){
        BTreePage* node=(BTreePage*)BufferPoolFetchPage(idx->bpm,curr_block);
        if(node==NULL){
            return false;
        }
        if(node->header.is_leaf){
            int pos=BTreeBinarySearch(node->keys,node->header.num_keys,key,&found);
            if(found&&outTid!=NULL){
                *outTid=node->tids[pos];
            }
            BufferPoolUnpinPage(idx->bpm,curr_block,false);
            return found;
        }
        int child_idx=BTreeBinarySearch(node->keys,node->header.num_keys,key,&found);
        if(found){
            child_idx++;
        }
        BlockNumber next_block=node->children[child_idx];
        BufferPoolUnpinPage(idx->bpm,curr_block,false);
        curr_block=next_block;
    }
    return false;
}
bool BTreeInsert(BTreeIndex* idx, int64_t key, ItemPointerData tid);
