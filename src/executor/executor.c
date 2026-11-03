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
//FilterExecutor
typedef struct FilterState {
    ExecutorNode* child;
    bool (*predicate)(Tuple*);
} FilterState;

static void FilterInit(ExecutorNode* self) {
    FilterState* state = (FilterState*)self->node_state;
    state->child->Init(state->child);
}

static Tuple* FilterNext(ExecutorNode* self) {
    FilterState* state = (FilterState*)self->node_state;

    while (1) {
        Tuple* tuple = state->child->Next(state->child);
        if (tuple == NULL) {
            return NULL; 
        }
        if (state->predicate(tuple)) {
            return tuple; 
        }
    }
}

static void FilterClose(ExecutorNode* self) {
    FilterState* state = (FilterState*)self->node_state;
    state->child->Close(state->child);
    free(state);
    free(self);
}

ExecutorNode* CreateFilterExecutor(ExecutorNode* child, bool (*predicate)(Tuple*)) {
    ExecutorNode* node = (ExecutorNode*)malloc(sizeof(ExecutorNode));
    FilterState* state = (FilterState*)malloc(sizeof(FilterState));

    state->child = child;
    state->predicate = predicate;

    node->Init = FilterInit;
    node->Next = FilterNext;
    node->Close = FilterClose;
    node->node_state = state;
    return node;
    //Limit
    typedef struct LimitState {
    ExecutorNode* child;
    size_t        limit;
    size_t        count;
} LimitState;

static void LimitInit(ExecutorNode* self) {
    LimitState* state = (LimitState*)self->node_state;
    state->child->Init(state->child);
    state->count = 0;
}

static Tuple* LimitNext(ExecutorNode* self) {
    LimitState* state = (LimitState*)self->node_state;

    if (state->count >= state->limit) {
        return NULL;
    }

    Tuple* tuple = state->child->Next(state->child);
    if (tuple == NULL) {
        return NULL;
    }

    state->count++;
    return tuple;
}

static void LimitClose(ExecutorNode* self) {
    LimitState* state = (LimitState*)self->node_state;
    state->child->Close(state->child);
    free(state);
    free(self);
}

ExecutorNode* CreateLimitExecutor(ExecutorNode* child, size_t limit) {
    ExecutorNode* node = (ExecutorNode*)malloc(sizeof(ExecutorNode));
    LimitState* state = (LimitState*)malloc(sizeof(LimitState));

    state->child = child;
    state->limit = limit;
    state->count = 0;

    node->Init = LimitInit;
    node->Next = LimitNext;
    node->Close = LimitClose;
    node->node_state = state;
    return node;
}
}
