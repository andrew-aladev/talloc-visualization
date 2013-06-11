#ifndef TALLOC_VISUALIZATION_EPOLL_H
#define TALLOC_VISUALIZATION_EPOLL_H

#include "socket.h"
#include "stdbool.h"

#include <errno.h>
#include <sys/epoll.h>

enum {
    TV_EVENT_TYPE_SOCKET = 0,
    TV_EVENT_TYPE_CONNECTION,
    TV_EVENT_TYPE_TIMER,
};

typedef struct tv_event_t {
    uint8_t type;
    void * data;
} tv_event;

uint8_t tv_sockets_wait    ( tv_sockets * sockets );
uint8_t tv_socket_wait     ( tv_socket  * socket );
uint8_t tv_connection_wait ( tv_sockets * sockets, tv_connection * connection );
uint8_t tv_timer_wait      ( tv_sockets * sockets );

#endif
