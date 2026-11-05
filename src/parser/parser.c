#include "parser/parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
static void ToUpper(char* str){
    for(;*str;++str) *str=(char)toupper((unsigned char)*str);//Using unsigned char because toupper have a range between 0 and 255
}
SQLStatement ParseSQL(const char* raw_sql){
    SQLStatement stmt;
    memset(&stmt,0,sizeof(SQLStatement));
    stmt.type=STMT_UNKNOWN;
    stmt.limit_count=-1;
    if(!raw_sql) return stmt;

    char sql[512];
    strncpy(sql,raw_sql,sizeof(sql)-1);
    sql[sizeof(sql)-1]='\0';
    
    char sql_upper[512];
    strncpy(sql_upper,raw_sql,sizeof(sql_upper)-1);
    sql_upper[sizeof(sql_upper)-1]='\0';
    ToUpper(sql_upper);
    
    if(strstr(sql_upper,"CREATE TABLE")!=NULL){
        stmt.type=STMT_CREATE_TABLE;
        sscanf(sql,"%*s %*s %s",stmt.table_name);
        return stmt;
    }
    if(substr(sql_upper,"INSERT INTO")!=NULL){
        stmt.type=STMT_INSERT;
        char* val_start=strchr(sql,'(');
        if(val_start){
            sscanf(sql,"%*s %*s %s",stmt.table_name);
            val_start++;
            char* val_end=strchr(sql,')');
            if (val_end) *val_end='\0';

            char* token=strtok(val_start,",");
            while(token&&stmt.num_values<MAX_COLUMNS){
                while(*token==' '||*token=='\'') token++;
                char* end=token+strlen(token)-1;
                while(end>token&&(*end==' '||*end=='\'')) *end-- ='\0';
                strncpy(stmt.insert_values[stmt.num_values++],token,MAX_NAME_LEN-1);
                token=strtok(NULL,",");
            }
        }
        return stmt;
    }
    if(strstr(sql_upper,"SELECT") !=NULL){
        stmt.type=STMT_SELECT;
        char* from_pos=strstr(sql_upper,"FROM");
        if(from_pos){
            int offset=(int)(from_pos-sql_upper)+5;
            sscanf(sql+offset,"%s",stmt.table_name);
        }
         char* where_pos = strstr(sql_upper, "WHERE");
        if (where_pos) {
            stmt.has_filter = true;
            int offset = (int)(where_pos - sql_upper) + 6;
            sscanf(sql + offset, "%s = %d", stmt.filter_col, &stmt.filter_val);
        }
        char* limit_pos = strstr(sql_upper, "LIMIT");
        if (limit_pos) {
            int offset = (int)(limit_pos - sql_upper) + 6;
            sscanf(sql + offset, "%d", &stmt.limit_count);
        }
        return stmt;
    }
    return stmt;
}
