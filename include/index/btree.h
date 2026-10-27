#ifndef INDEX_BTREE_H
#define INDEX_BTREE_H

#include "common/types.h"

typedef struct BTreePageHeader {
    bool        is_leaf;
    uint16_t    num_keys;
    BlockNumber next_leaf;   /* Sibling link for range scans */
    BlockNumber prev_leaf;
} BTreePageHeader;

typedef struct BTreeIndex {
    BlockNumber root_block;
    const char* index_name;
} BTreeIndex;

bool BTreeInsert(BTreeIndex* idx, int64_t key, ItemPointerData tid);
bool BTreeFind(BTreeIndex* idx, int64_t key, ItemPointerData* outTid);

#endif /* INDEX_BTREE_H */
