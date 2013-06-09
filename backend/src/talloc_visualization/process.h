#ifndef TALLOC_VISUALIZATION_PROCESS_H
#define TALLOC_VISUALIZATION_PROCESS_H

#include "socket.h"

void    tv_read   ( tv_connection * connection );
uint8_t tv_header ( tv_connection * connection );

#endif
