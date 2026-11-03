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
