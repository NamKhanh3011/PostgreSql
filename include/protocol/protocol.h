#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <winsock2.h>
#include <stdbool.h>
#include "catalog/catalog.h"

/* Giao thức Postgres v3.0 packet helpers */
void SendAuthOk(SOCKET fd);
void SendReadyForQuery(SOCKET fd);
void SendRowDescription(SOCKET fd, TableSchema* schema);
void SendDataRow(SOCKET fd, const char* values[], int num_vals);
void SendCommandComplete(SOCKET fd, const char* tag);
void SendErrorResponse(SOCKET fd, const char* msg);

#endif
