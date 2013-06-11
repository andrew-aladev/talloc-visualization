#ifndef TALLOC_VISUALIZATION_PROCESS_H
#define TALLOC_VISUALIZATION_PROCESS_H

#include "socket.h"

void tv_process ( tv_connection * connection );

uint8_t tv_read  ( tv_connection * connection );
uint8_t tv_write ( tv_connection * connection, int flags );

#endif
