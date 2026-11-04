#include <stdio.h>
#include "catalog/catalog.h"
#include <stdlib.h>
#include <string.h>
Catalog* CatalogCreate(void){
    Catalog* cat =(Catalog*)malloc(sizeof(Catalog));
    if(cat){
        cat->num_tables=0;
    }
    return cat;
}
bool CatalogAddTable(Catalog* cat, const char* name, Column* cols, int num_cols){
    if(!cat||cat->num_tables>=MAX_TABLES||num_cols>MAX_COLUMNS) return false;
    TableSchema* tbl = &cat->tables[cat->num_tables++];
    strncpy(tbl->table_name,name,MAX_NAME_LEN-1);
    tbl->table_name[MAX_NAME_LEN-1]='\0';
    tbl->num_columns=num_cols;
    tbl->total_blocks=0;
    for(int i=0;i<num_cols;i++){
        tbl->columns[i]=cols[i];
    }
    return true;
}
TableSchema* CatalogGetTable(Catalog* cat, const char* name){
    if(!cat||!name) return NULL;
    for(int i=0;i<cat->num_tables;i++){
        if(strcmp(cat->tables[i].table_name,name)==0){
            return &cat->tables[i];
        }
    }
    return NULL;
}
