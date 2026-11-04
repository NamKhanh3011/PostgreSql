#ifndef PARSER_H
#define PARSER_H

#include "catalog/catalog.h"
#include <stdbool.h>

typedef enum {
    STMT_SELECT,
    STMT_INSERT,
    STMT_CREATE_TABLE,
    STMT_UNKNOWN
} StatementType;

typedef struct SQLStatement {
    StatementType type;
    char table_name[MAX_NAME_LEN];

    //Insert
    char insert_values[MAX_COLUMNS][MAX_NAME_LEN];
    int num_values;

    //Select
    bool has_filter;
    char filter_col[MAX_NAME_LEN];
    int filter_val;
    int limit_count; 
} SQLStatement;

SQLStatement ParseSQL(const char* sql);

#endif
