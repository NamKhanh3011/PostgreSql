#include <stdio.h>
#include <stdlib.h>
#include "common/types.h"
#include "storage/page.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("==================================================\n");
    printf("   Mini-PostgreSQL Database Engine (Pure C11)     \n");
    printf("   Standard Block Size: %d bytes (Slotted-Page)   \n", BLCKSZ);
    printf("==================================================\n\n");

    printf("[1] Initializing Slotted-Page verification...\n");
    char page_buffer[BLCKSZ];
    PageInit(page_buffer, BLCKSZ);

    const char* sample_row = "PostgreSQL From Scratch - Core Row Payload";
    LocationIndex item_idx = 0;
    if (PageAddItem(page_buffer, sample_row, 43, &item_idx)) {
        printf("  [+] Tuple appended into page at slot index: %u\n", item_idx);
    }

    size_t out_size = 0;
    char* retrieved = PageGetItem(page_buffer, item_idx, &out_size);
    if (retrieved) {
        printf("  [+] Retrieved tuple from slotted page: \"%s\" (%zu bytes)\n", retrieved, out_size);
    }

    printf("\n[Ready] Project structure compiled. Ready to implement Phase 1 to Phase 7!\n");
    return 0;
}
