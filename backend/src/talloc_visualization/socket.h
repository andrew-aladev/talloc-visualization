#ifndef TALLOC_VISUALIZATION_LISTEN_H
#define TALLOC_VISUALIZATION_LISTEN_H

#include "utils/list.h"

#include <unistd.h>

#include <talloc2/ext/destructor.h>
#include <talloc2/utils/buffer.h>

typedef struct tv_sockets_t {
    tv_list * socket_list;
    int epoll_fd;
    struct epoll_event * events;
} tv_sockets;

typedef struct tv_socket_t {
    int fd;
    tv_list * connection_list;
} tv_socket;

typedef struct tv_connection_t {
    int fd;
    char * host;
    char * port;
    talloc_buffer * buffer;
} tv_connection;

uint8_t tv_bind   ( tv_sockets * sockets, const char * port );
void    tv_accept ( tv_sockets * sockets, tv_socket  * socket );

inline
uint8_t tv_sockets_close ( void * data )
{
    tv_sockets * sockets = data;
    if ( sockets->epoll_fd != -1 ) {
        if ( close ( sockets->epoll_fd ) != 0 ) {
            return 1;
        }
    }
    return 0;
}

inline
tv_sockets * tv_sockets_new ( void * ctx )
{
    tv_sockets * sockets = talloc ( ctx, sizeof ( tv_sockets ) );
    if ( sockets == NULL ) {
        return NULL;
    }
    tv_list * list = tv_list_new ( sockets );
    if ( list == NULL ) {
        talloc_free ( sockets );
        return NULL;
    }
    sockets->socket_list = list;
    sockets->epoll_fd    = -1;
    if ( talloc_set_destructor ( sockets, tv_sockets_close ) != 0 ) {
        talloc_free ( sockets );
        return NULL;
    }
    return sockets;
}

inline
uint8_t tv_socket_close ( void * data )
{
    tv_socket * socket = data;
    if ( socket->fd != -1 ) {
        if ( close ( socket->fd ) != 0 ) {
            return 1;
        }
    }
    return 0;
}

inline
tv_socket * tv_socket_new ( void * ctx )
{
    tv_socket * socket = talloc ( ctx, sizeof ( tv_socket ) );
    if ( socket == NULL ) {
        return NULL;
    }
    tv_list * list = tv_list_new ( socket );
    if ( list == NULL ) {
        talloc_free ( socket );
        return NULL;
    }
    socket->connection_list = list;
    socket->fd              = -1;
    if ( talloc_set_destructor ( socket, tv_socket_close ) != 0 ) {
        talloc_free ( socket );
        return NULL;
    }
    return socket;
}

inline
uint8_t tv_connection_close ( void * data )
{
    tv_connection * connection = data;
    if ( connection->fd != -1 ) {
        if ( close ( connection->fd ) != 0 ) {
            return 1;
        }
    }
    return 0;
}

inline
tv_connection * tv_connection_new ( void * ctx )
{
    tv_connection * connection = talloc ( ctx, sizeof ( tv_connection ) );
    if ( connection == NULL ) {
        return NULL;
    }
    connection->fd     = -1;
    connection->host   = NULL;
    connection->port   = NULL;
    connection->buffer = NULL;
    if ( talloc_set_destructor ( connection, tv_connection_close ) != 0 ) {
        talloc_free ( connection );
        return NULL;
    }
    return connection;
}

#endif

