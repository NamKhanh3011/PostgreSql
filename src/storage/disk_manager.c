#include <stdio.h>
#include <stdbool.h>
#include "common/types.h"

typedef struct DiskManager{
    FILE* db_file;
    int file_pages;
}DiskManager;

DiskManager* DiskManagerOpen(const char* db_file_path){
    DiskManager* dm=(DiskManager*) malloc(sizeof(DiskManager));
    if(dm==NULL){
        return NULL;
    }
    dm->db_file=fopen(db_file_path,"rb+");
    if(dm->db_file==NULL){
        dm->db_file=fopen(db_file_path,"wb+");
        if(dm->db_file==NULL){
            free(dm);
            return NULL;
        }
    }
    fseek(dm->db_file,0,SEEK_END);
    long chill=ftell(dm->db_file);
    dm->file_pages=(int)(chill/BLCKSZ);
    fseek(dm->db_file,0,SEEK_SET);
    return dm;
}
