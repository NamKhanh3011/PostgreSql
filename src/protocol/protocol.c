#include "protocol/protocol.h"
#include <string.h>

static void SendBytes(SOCKET fd, const char* buf, int len) {
    send(fd, buf, len, 0);
}
void SendAuthOk(SOCKET fd) {
    char packet[9];
    packet[0] = 'R';
    int len = htonl(8);
    int code = htonl(0);
    memcpy(packet + 1, &len, 4);
    memcpy(packet + 5, &code, 4);
    SendBytes(fd, packet, 9);
}
void SendReadyForQuery(SOCKET fd) {
    char packet[6];
    packet[0] = 'Z';
    int len = htonl(5);
    char status = 'I';
    memcpy(packet + 1, &len, 4);
    packet[5] = status;
    SendBytes(fd, packet, 6);
}
void SendRowDescription(SOCKET fd, TableSchema* schema) {
    char buf[1024];
    buf[0] = 'T';
    int p = 5; 

    int16_t num_fields = htons((int16_t)schema->num_columns);
    memcpy(buf + p, &num_fields, 2);
    p += 2;

    for (int i = 0; i < schema->num_columns; i++) {
        int name_len = (int)strlen(schema->columns[i].name) + 1;
        memcpy(buf + p, schema->columns[i].name, name_len);
        p += name_len;

        int32_t table_oid = htonl(0);
        int16_t col_attr = htons(i + 1);
        int32_t type_oid = htonl(schema->columns[i].type == TYPE_INT ? 23 : 1043);
        int16_t type_len = htons(schema->columns[i].type == TYPE_INT ? 4 : -1);
        int32_t type_mod = htonl(-1);
        int16_t format = htons(0);

        memcpy(buf + p, &table_oid, 4); p += 4;
        memcpy(buf + p, &col_attr, 2);  p += 2;
        memcpy(buf + p, &type_oid, 4);  p += 4;
        memcpy(buf + p, &type_len, 2);  p += 2;
        memcpy(buf + p, &type_mod, 4);  p += 4;
        memcpy(buf + p, &format, 2);    p += 2;
    }

    int total_len = htonl(p - 1);
    memcpy(buf + 1, &total_len, 4);
    SendBytes(fd, buf, p);
}
void SendDataRow(SOCKET fd, const char* values[], int num_vals) {
    char buf[2048];
    buf[0] = 'D';
    int p = 5;

    int16_t cols = htons((int16_t)num_vals);
    memcpy(buf + p, &cols, 2);
    p += 2;

    for (int i = 0; i < num_vals; i++) {
        int vlen = (int)strlen(values[i]);
        int32_t len_be = htonl(vlen);
        memcpy(buf + p, &len_be, 4);
        p += 4;
        memcpy(buf + p, values[i], vlen);
        p += vlen;
    }

    int total_len = htonl(p - 1);
    memcpy(buf + 1, &total_len, 4);
    SendBytes(fd, buf, p);
}
void SendCommandComplete(SOCKET fd, const char* tag) {
    char buf[128];
    buf[0] = 'C';
    int tag_len = (int)strlen(tag) + 1;
    int total_len = htonl(tag_len + 4);
    memcpy(buf + 1, &total_len, 4);
    memcpy(buf + 5, tag, tag_len);
    SendBytes(fd, buf, tag_len + 5);
}
void SendErrorResponse(SOCKET fd, const char* msg) {
    char buf[512];
    buf[0] = 'E';
    int p = 5;
    buf[p++] = 'S'; 
    strcpy(buf + p, "ERROR"); p += 6;
    buf[p++] = 'M'; 
    strcpy(buf + p, msg); p += (int)strlen(msg) + 1;
    buf[p++] = '\0'; 

    int total_len = htonl(p - 1);
    memcpy(buf + 1, &total_len, 4);
    SendBytes(fd, buf, p);
}
