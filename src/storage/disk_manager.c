#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common/types.h"
#include "storage/page.h"

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
void DiskManagerClose(DiskManager* dm){
    if(dm==NULL){
        return;
    }
    if(dm->db_file!=NULL){
        fclose(dm->db_file);
    }
    free(dm);
}
bool DiskManagerReadPage(DiskManager* dm,BlockNumber block_num,Page page_out){
    if(block_num>=dm->file_pages){
        return false;
    }
    long offset=(long)block_num*BLCKSZ;
    fseek(dm->db_file,offset,SEEK_SET);
    size_t byteread=fread(page_out,1,BLCKSZ,dm->db_file);
    if(byteread!=BLCKSZ){
        return false;
    }
    return true;
}
bool DiskManagerWritePage(DiskManager* dm,BlockNumber block_num,const Page page_in){
    long offset=(long)block_num*BLCKSZ;
    fseek(dm->db_file,offset,SEEK_SET);
    size_t bytewrite=fwrite(page_in,1,BLCKSZ,dm->db_file);
    size_t flush=fflush(dm->db_file);
    if(bytewrite!=BLCKSZ){
        return false;
    }
    return true;
}
BlockNumber DiskManagerAllocatePage(DiskManager* dm){
    BlockNumber new_page_id=dm->file_pages;
    char empty_page[BLCKSZ]={0};
    DiskManagerWritePage(dm,new_page_id,empty_page);
    if(!DiskManagerWritePage(dm,new_page_id,empty_page)){
        return InvalidBlockNumber;
    }
    dm->file_pages++;
    return new_page_id;
}
