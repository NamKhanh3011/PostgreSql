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
bool BTreeInsert(BTreeIndex* idx, int64_t key, ItemPointerData tid){
    if(idx==NULL) return false;
    if(idx->root_block==InvalidBlockNumber){
        BlockNumber new_root=DiskManagerAllocatePage(idx->bpm->disk_manager);
        BTreePage* root=(BTreePage*)BufferPoolFetchPage(idx->bpm,new_root);
        if(root==NULL) return false;
        root->header.is_leaf=true;
        root->header.num_keys=1;
        root->header.next_leaf=InvalidBlockNumber;
        root->header.prev_leaf=InvalidBlockNumber;
        root->keys[0]=key;
        root->tids[0]=tid;
        BufferPoolUnpinPage(idx->bpm,new_root,true);
        idx->root_block=new_root;
        return true;
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
            if(found){
            node->tids[pos]=tid;
            BufferPoolUnpinPage(idx->bpm,curr_block,true);
            return true;
        }
        //Not full->Add more
        if(node->header.num_keys<BTREE_MAX_KEYS){
            for(int i=node->header.num_keys;i>pos;i--){
                node->keys[i]=node->keys[i-1];
                node->tids[i]=node->tids[i-1];
            }
            node->keys[pos]=key;
            node->tids[pos]=tid;
            node->header.num_keys++;
            BufferPoolUnpinPage(idx->bpm,curr_block,true);
            return true;
        }
        //Full->Split
        BlockNumber new_leaf_blk = DiskManagerAllocatePage(idx->bpm->disk_manager);
        BTreePage* new_leaf = (BTreePage*)BufferPoolFetchPage(idx->bpm, new_leaf_blk);
        if(new_leaf==NULL){
            BufferPoolUnpinPage(idx->bpm,curr_block,false);
            return false;
        }
        new_leaf->header.is_leaf=true;
        new_leaf->header.next_leaf=node->header.next_leaf;
        new_leaf->header.prev_leaf=curr_block;
        node->header.next_leaf=new_leaf_blk;

        int split_point=node->header.num_keys/2;
        int move_count=node->header.num_keys-split_point;

        for(int i=0;i<move_count;i++){
            new_leaf->keys[i]=node->keys[split_point+i];
            new_leaf->tids[i]=node->tids[split_point+i];
        }
        new_leaf->header.num_keys=move_count;
        node->header.num_keys=split_point;
        if(key<new_leaf->keys[0]){
            int pos=BTreeBinarySearch(node->keys,node->header.num_keys,key,&found);
            for(int i=node->header.num_keys;i>pos;i--){
                node->keys[i]=node->keys[i-1];
                node->tids[i]=node->tids[i-1];
            }
            node->keys[pos]=key;
            node->tids[pos]=tid;
            node->header.num_keys++;
        }
        else{
            int pos=BTreeBinarySearch(new_leaf->keys,new_leaf->header.num_keys,key,&found);
            for(int i=new_leaf->header.num_keys,i>pos;i--){
                new_leaf->keys[i]=new_leaf->keys[i-1];
                new_leaf->tids[i]=new_leaf->tids[i-1];
            }
            new_leaf->keys[pos] = key;
            new_leaf->tids[pos] = tid;
            new_leaf->header.num_keys++;
        }
        BufferPoolUnpinPage(idx->bpm,new_leaf_blk,true);
        BufferPoolUnpinPage(idx->bpm,curr_block,true);
        return true;
    }
    int child_idx = BTreeBinarySearch(node->keys, node->header.num_keys, key, &found);
    if (found) child_idx++;
    BlockNumber next_block = node->children[child_idx];
    BufferPoolUnpinPage(idx->bpm, curr_block, false);
    curr_block = next_block;
}
return false;
}
