#ifndef TALLOC_VISUALIZATION_LISTEN_H
#define TALLOC_VISUALIZATION_LISTEN_H

#include "utils/list.h"

#include <unistd.h>

#include <talloc2/ext/destructor.h>
#include <talloc2/utils/buffer.h>

#include <wslay/wslay.h>
#include <wslay/event.h>

typedef struct tv_sockets_t {
    tv_list * socket_list;
    int timer_fd;
    int epoll_fd;
    struct epoll_event * events;
} tv_sockets;

typedef struct tv_socket_t {
    int fd;
    tv_list * connection_list;
} tv_socket;

enum {
    TV_CONNECTION_STATUS_RAW = 0,
    TV_CONNECTION_STATUS_HANDSHAKE_SENDING,
    TV_CONNECTION_STATUS_HANDSHAKE_SENDED
};

typedef uint8_t ( * tv_callback ) ( void * data );

typedef struct tv_connection_t {
    tv_list * connection_list;

    int fd;
    uint8_t status;
    talloc_buffer * request;
    char * upgrade;
    size_t upgrade_length;
    char * key;
    size_t key_length;
    char * version;
    size_t version_length;

    void * response;
    void * response_ptr;
    size_t response_length;

    tv_callback read_callback;
    tv_callback write_callback;

    wslay_event_context * events;

    size_t talloc_events_buffer_length;
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
    if ( sockets->timer_fd != -1 ) {
        if ( close ( sockets->timer_fd ) != 0 ) {
            return 2;
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
    sockets->timer_fd    = -1;
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

    tv_list_item * connection_item = connection->connection_list->last_item;
    while ( connection_item != NULL ) {
        if ( connection_item->data == connection ) {
            tv_list_delete ( connection->connection_list, connection_item );
            break;
        }
        connection_item = connection_item->prev;
    }

    return 0;
}

inline
tv_connection * tv_connection_new ( tv_list * connection_list )
{
    tv_connection * connection = talloc ( connection_list, sizeof ( tv_connection ) );
    if ( connection == NULL ) {
        return NULL;
    }
    connection->connection_list = connection_list;

    connection->fd      = -1;
    connection->status  = TV_CONNECTION_STATUS_RAW;
    connection->request = NULL;

    connection->upgrade = NULL;
    connection->key     = NULL;
    connection->version = NULL;
    connection->upgrade_length = 0;
    connection->key_length     = 0;
    connection->version_length = 0;

    connection->response        = NULL;
    connection->response_ptr    = NULL;
    connection->response_length = 0;

    connection->read_callback  = NULL;
    connection->write_callback = NULL;

    connection->events = NULL;

    connection->talloc_events_buffer_length = 0;

    if ( talloc_set_destructor ( connection, tv_connection_close ) != 0 ) {
        talloc_free ( connection );
        return NULL;
    }
    return connection;
}

#endif

