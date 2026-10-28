#include <stdio.h>
#include <stdbool.h>
#include "common/types.h"

typedef struct DiskManager{
    FILE* db_file;
    int file_pages;
}DiskManager;

DiskManager* DiskManagerOpen(const char* file_path){
    DiskManager* dm=(DiskManager*) malloc(sizeof(DiskManager));
    if(dm==NULL){
        return NULL;
    }
    
}
