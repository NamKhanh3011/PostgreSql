#ifndef CATALOG_H
#define CATALOG_H

#include "common/types.h"
#include <stdbool.h>

#define MAX_COLUMNS   16
#define MAX_NAME_LEN  64
#define MAX_TABLES    16

typedef enum {
    TYPE_INT,
    TYPE_VARCHAR
} ColumnType;

typedef struct Column {
    char name[MAX_NAME_LEN];
    ColumnType type;
    int length;
} Column;

typedef struct TableSchema {
    char table_name[MAX_NAME_LEN];
    int num_columns;
    Column columns[MAX_COLUMNS];
    BlockNumber total_blocks;
} TableSchema;

typedef struct Catalog {
    TableSchema tables[MAX_TABLES];
    int num_tables;
} Catalog;

Catalog* CatalogCreate(void);
bool CatalogAddTable(Catalog* cat, const char* name, Column* cols, int num_cols);
TableSchema* CatalogGetTable(Catalog* cat, const char* name);

#endif
