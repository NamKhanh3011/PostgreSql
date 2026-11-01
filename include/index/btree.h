#ifndef INDEX_BTREE_H
#define INDEX_BTREE_H

#include "common/types.h"

typedef struct BTreePageHeader {
    bool        is_leaf;
    uint16_t    num_keys;
    BlockNumber next_leaf;   /* Sibling link for range scans */
    BlockNumber prev_leaf;
} BTreePageHeader;

typedef struct BTreePage {
    BTreePageHeader header;
    int64_t         keys[BTREE_MAX_KEYS];
    union {
        ItemPointerData tids[BTREE_MAX_KEYS];         
        BlockNumber     children[BTREE_MAX_KEYS + 1]; 
    };
} BTreePage;

typedef struct BTreeIndex {
    BlockNumber root_block;
    const char* index_name;
} BTreeIndex;
void BTreeInit(BTreeIndex* idx, const char* name, BufferPoolManager* bpm);
bool BTreeInsert(BTreeIndex* idx, int64_t key, ItemPointerData tid);
bool BTreeFind(BTreeIndex* idx, int64_t key, ItemPointerData* outTid);

#endif /* INDEX_BTREE_H */
