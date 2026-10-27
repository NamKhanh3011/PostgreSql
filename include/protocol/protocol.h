#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/* PostgreSQL Frontend/Backend Wire Protocol v3.0 message identifiers */
#define PG_MSG_QUERY            'Q'
#define PG_MSG_ROW_DESCRIPTION  'T'
#define PG_MSG_DATA_ROW         'D'
#define PG_MSG_COMMAND_COMPLETE 'C'
#define PG_MSG_READY_FOR_QUERY  'Z'
#define PG_MSG_ERROR_RESPONSE   'E'

void ProtocolStartServer(int port);

#endif /* PROTOCOL_H */
