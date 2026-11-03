#include "executor/executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//SeqScanExecutor
typedef struct SeqScanState{
    BufferPoolManager* bpm;
    BlockNumber        total_block;
    BlockNumber        curr_block;
    LocationIndex      curr_slot;
    Page               curr_page;
    Tuple              current_tuple;
} SeqScanState
static void SeqScanInit(ExecutorNode* self){
    SeqScanState* state=(SeqScanState*)self->node_state;
    state->curr_block=0;
    state->curr_slot=0;
    state->curr_page=NULL;
    if(state->total_block>0){
        state->curr_page=BufferPoolFetchPage(state->bpm,0);
    }
}
static Tuple* SeqScanNext(ExecutorNode* self){
    SeqScanState* state= (SeqScanState*)self->node_state;
    while(state->curr_block<state->total_block){
        if(state->curr_page==NULL){
            state->curr_page=BufferPoolFetchPage(state->bpm,state->curr_block)
        if(state->curr_page==NULL) return NULL;
        state->curr_slot=0;
    }
    PageHeaderData* hdr=(PageHeaderData*)state->curr_page;
    size_t total_items=(hdr->pd_lower-sizeof(PageHeaderData))/sizeof(ItemIdData);
    while(state->curr_slot<total_items){
        size_t item_size=0;
        char* item_data=PageGetItem(state->curr_page,state->curr_slot,&item);
        state->curr_slot++;
        if (item_data != NULL) {
                state->current_tuple.data = item_data;
                state->current_tuple.size = item_size;
                return &state->current_tuple;}
    }
    BufferPoolUnpinPage(state->bpm, state->curr_block, false);
        state->curr_page = NULL;
        state->curr_block++;
        state->curr_slot = 0;
}
return NULL;
}
static void SeqScanClose(ExecutorNode* self){
    SeqScanState* state = (SeqScanState*)self->node_state;
    if(self->curr_page!=NULL){
        BufferPoolUnpinPage(state->bpm,state->curr_block,false);
    }
    free(state);
    free(self);
}
ExecutorNode* CreateSeqScanExecutor(BufferPoolManager* bpm,BlockNumber total_blocks){
    ExecutorNode* node=(ExecutorNode*)malloc(sizeof(ExecutorNode));
    SeqScanState* state=(SeqScanState*)malloc(sizeof(SeqScanState));
    state->bpm=bpm;
    state->total_blocks=total_blocks;
    state->curr_block=0;
    state->curr_slot=0;
    state->curr_page=NULL;

    node->Init=SeqScanInit;
    node->Next=SeqScanNext;
    node->Close=SeqScanClose;
    node->node_state=state;
    return node;
}
