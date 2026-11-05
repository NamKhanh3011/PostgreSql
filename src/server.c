#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "storage/disk_manager.h"
#include "storage/buffer_pool.h"
#include "catalog/catalog.h"
#include "parser/parser.h"
#include "executor/executor.h"
#include "wal/wal.h"
#include "protocol/protocol.h"

#define PORT 5432

int main(void) {
    printf("===================================================\n");
    printf("     MINI-POSTGRESQL DATABASE SERVER ENGINE        \n");
    printf("===================================================\n");

    BufferPoolManager* bpm = BufferPoolCreate("mini_pg.db");
    Catalog* catalog = CatalogCreate();

    /* Register default system table "users" */
    Column user_cols[2] = {
        {"id", TYPE_INT, 4},
        {"name", TYPE_VARCHAR, 50}
    };
    CatalogAddTable(catalog, "users", user_cols, 2);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("[+] Server listening on port %d (Ready for connections!)...\n", PORT);
    fflush(stdout);

    while (1) {
        SOCKET client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == INVALID_SOCKET) continue;
        printf("\n[+] Client connected to server!\n");
        fflush(stdout);

        int32_t len = 0;
        recv(client_fd, (char*)&len, 4, 0);
        len = ntohl(len);

        int32_t code = 0;
        recv(client_fd, (char*)&code, 4, 0);
        code = ntohl(code);

        /* SSL negotiation request probe */
        if (code == 80877103) {
            char ssl_no = 'N';
            send(client_fd, &ssl_no, 1, 0);

            recv(client_fd, (char*)&len, 4, 0);
            char discard[512];
            recv(client_fd, discard, ntohl(len) - 4, 0);
        } else {
            char discard[512];
            recv(client_fd, discard, len - 8, 0);
        }

        SendAuthOk(client_fd);
        SendReadyForQuery(client_fd);

        char type = 0;
        while (recv(client_fd, &type, 1, 0) > 0) {
            if (type == 'X') break; /* Client termination */

            if (type == 'Q') {
                int32_t qlen = 0;
                recv(client_fd, (char*)&qlen, 4, 0);
                qlen = ntohl(qlen) - 4;

                char sql[512];
                memset(sql, 0, sizeof(sql));
                if (qlen > 0 && qlen < (int)sizeof(sql)) {
                    recv(client_fd, sql, qlen, 0);
                    sql[qlen] = '\0';
                }
                printf("[SQL Received]: %s\n", sql);
                fflush(stdout);

                SQLStatement stmt = ParseSQL(sql);
                TableSchema* tbl = CatalogGetTable(catalog, stmt.table_name);

                if (stmt.type == STMT_CREATE_TABLE) {
                    SendCommandComplete(client_fd, "CREATE TABLE");
                } 
                else if (stmt.type == STMT_INSERT) {
                    if (tbl == NULL) {
                        SendErrorResponse(client_fd, "Table does not exist!");
                    } else {
                        const char* val_id   = stmt.num_values > 0 ? stmt.insert_values[0] : "0";
                        const char* val_name = stmt.num_values > 1 ? stmt.insert_values[1] : "";
                        WalAppendRecord(WAL_INSERT_TUPLE, 100, val_name, strlen(val_name));

                        if (tbl->total_blocks == 0) {
                            DiskManagerAllocatePage(bpm->disk_manager);
                            tbl->total_blocks = 1;
                            Page p = BufferPoolFetchPage(bpm, 0);
                            if (p) {
                                PageInit(p, BLCKSZ);
                                BufferPoolUnpinPage(bpm, 0, true);
                            }
                        }

                        char record_data[128];
                        snprintf(record_data, sizeof(record_data), "%s|%s", val_id, val_name);
                        
                        Page p = BufferPoolFetchPage(bpm, 0);
                        if (p) {
                            LocationIndex slot_idx = 0;
                            PageAddItem(p, record_data, (size_t)strlen(record_data) + 1, &slot_idx);
                            BufferPoolUnpinPage(bpm, 0, true);
                        }

                        SendCommandComplete(client_fd, "INSERT 0 1");
                    }
                } 
                else if (stmt.type == STMT_SELECT) {
                    if (tbl == NULL) {
                        SendErrorResponse(client_fd, "Table does not exist!");
                    } else {
                        SendRowDescription(client_fd, tbl);

                        /* Construct Volcano pipeline */
                        ExecutorNode* scan = CreateSeqScanExecutor(bpm, tbl->total_blocks);
                        ExecutorNode* pipeline = scan;
                        if (stmt.limit_count > 0) {
                            pipeline = CreateLimitExecutor(scan, (size_t)stmt.limit_count);
                        }

                        pipeline->Init(pipeline);
                        Tuple* tuple = NULL;
                        int count = 0;

                        while ((tuple = pipeline->Next(pipeline)) != NULL) {
                            char temp[128];
                            strncpy(temp, tuple->data, sizeof(temp));
                            temp[sizeof(temp) - 1] = '\0';
                            
                            char* id = strtok(temp, "|");
                            char* name = strtok(NULL, "|");
                            if (id && name) {
                                const char* cols[] = {id, name};
                                SendDataRow(client_fd, cols, 2);
                                count++;
                            }
                        }
                        pipeline->Close(pipeline);

                        char tag[64];
                        snprintf(tag, sizeof(tag), "SELECT %d", count);
                        SendCommandComplete(client_fd, tag);
                    }
                } else {
                    SendCommandComplete(client_fd, "OK");
                }

                SendReadyForQuery(client_fd);
            }
        }

        closesocket(client_fd);
        printf("[-] Client disconnected.\n");
        fflush(stdout);
    }

    BufferPoolDestroy(bpm);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
