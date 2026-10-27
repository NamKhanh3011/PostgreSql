#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "common/types.h"
#include "storage/page.h"

int main(void) {
    printf("Running Slotted-Page unit tests...\n");
    char page[BLCKSZ];
    PageInit(page, BLCKSZ);

    const char* val = "Test Tuple";
    LocationIndex idx = 0;
    bool ok = PageAddItem(page, val, strlen(val) + 1, &idx);
    assert(ok == true);
    assert(idx == 0);

    size_t sz = 0;
    char* read_back = PageGetItem(page, idx, &sz);
    assert(read_back != NULL);
    assert(strcmp(read_back, val) == 0);

    printf("All Slotted-Page unit tests passed!\n");
    return 0;
}
