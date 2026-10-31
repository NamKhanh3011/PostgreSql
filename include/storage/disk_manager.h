#ifndef STORAGE_DISK_MANAGER_H
#define STORAGE_DISK_MANAGER_H

#include <stdio.h>
#include <stdbool.h>
#include "common/types.h"
#include "storage/page.h"

typedef struct DiskManager {
    FILE* db_file;
    int   file_pages;
} DiskManager;

DiskManager* DiskManagerOpen(const char* db_file_path);
void DiskManagerClose(DiskManager* dm);
bool DiskManagerReadPage(DiskManager* dm, BlockNumber block_num, Page page_out);
bool DiskManagerWritePage(DiskManager* dm, BlockNumber block_num, const Page page_in);
BlockNumber DiskManagerAllocatePage(DiskManager* dm);

#endif /* STORAGE_DISK_MANAGER_H */
