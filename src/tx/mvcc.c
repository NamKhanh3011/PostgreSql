#include "tx/mvcc.h"
#include <stdio.h>

bool HeapTupleSatisfiesVisibility(HeapTupleHeaderData* tuple_hdr,Snapshot snapshot){
    if(!tuple_hdr|| !snapshot) return false;
    if(tuple_hdr->t_xmin>=snapshot->xmax){
        return false;
    }
    if(tuple_hdr->t_max!= InvalidTransactionId && tuple_hdr->t_xmax<snapshot->xmin){
        return false;
    }
    return true;
}
