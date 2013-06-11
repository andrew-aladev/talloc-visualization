#include "process.h"
#include "socket.h"
#include "handshake.h"
#include "events.h"

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 512

void tv_process ( tv_connection * connection )
{
    switch ( connection->status ) {
    case TV_CONNECTION_STATUS_RAW:
        connection->read_callback = tv_handshake;
        if ( tv_read ( connection ) != 0 ) {
            talloc_free ( connection );
        }
        break;
    case TV_CONNECTION_STATUS_HANDSHAKE_SENDING:
        if ( tv_write ( connection, 0 ) != 0 ) {
            talloc_free ( connection );
        }
        break;
    case TV_CONNECTION_STATUS_HANDSHAKE_SENDED:
        if ( tv_write ( connection, 0 ) != 0 ) {
            talloc_free ( connection );
        }
        break;
    }
}

uint8_t tv_read ( tv_connection * connection )
{
    talloc_buffer * request = connection->request;
    if ( request == NULL ) {
        request = connection->request = talloc_buffer_new ( connection );
        if ( request == NULL ) {
            return 1;
        }
    }

    ssize_t count;
    while ( true ) {
        char * buf = talloc_buffer_get ( request, BUFFER_SIZE );
        if ( buf == NULL ) {
            talloc_free ( request );
            return 2;
        }
        count = read ( connection->fd, buf, BUFFER_SIZE );
        if ( count == -1 ) {
            if ( errno == EAGAIN || errno == EWOULDBLOCK ) {
                talloc_buffer_cut ( request, BUFFER_SIZE );
                break;
            } else {
                return 3;
            }
        } else if ( count == 0 ) {
            talloc_buffer_cut ( request, BUFFER_SIZE );
            talloc_free ( request );
            return 4;
        } else if ( count != BUFFER_SIZE ) {
            talloc_buffer_cut ( request, BUFFER_SIZE - count );

            tv_callback callback = connection->read_callback;
            if ( callback != NULL && callback ( connection ) != 0 ) {
                return 5;
            }
            connection->read_callback = NULL;
            break;
        }
    }

    return 0;
}

uint8_t tv_write ( tv_connection * connection, int flags )
{
    size_t response_length = connection->response_length;
    if ( response_length == 0 ) {
        return 1;
    }

    flags |= MSG_DONTWAIT;

    ssize_t sended_length = send ( connection->fd, connection->response_ptr, response_length, flags );
    if ( sended_length == -1 ) {
        if ( errno != EAGAIN && errno != EWOULDBLOCK ) {
            return 2;
        }
    } else {
        response_length -= sended_length;
        if ( response_length == 0 ) {
            talloc_free ( connection->response );
            connection->response        = NULL;
            connection->response_ptr    = NULL;
            connection->response_length = 0;

            tv_callback callback = connection->write_callback;
            if ( callback != NULL && callback ( connection ) != 0 ) {
                return 3;
            }
            connection->write_callback = NULL;
        }
        connection->response_ptr    += sended_length;
        connection->response_length = response_length;
    }

    return 0;
}
