#ifndef TALLOC_VISUALIZATION_EPOLL_H
#define TALLOC_VISUALIZATION_EPOLL_H

#include "socket.h"
#include "stdbool.h"

#include <errno.h>
#include <sys/epoll.h>

typedef struct tv_event_t {
    bool is_socket; // socket or connection
    void * data;
} tv_event;

uint8_t tv_sockets_wait ( tv_sockets * sockets );
uint8_t tv_socket_wait  ( tv_socket  * socket );
uint8_t tv_connection_wait ( tv_sockets * sockets, tv_connection * connection );

#endif
