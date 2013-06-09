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
        if ( tv_read ( connection, tv_handshake ) != 0 ) {
            talloc_free ( connection );
        }
        break;
    case TV_CONNECTION_STATUS_HANDSHAKE_SENDING:
        if ( tv_write ( connection, tv_handshake_sended, 0 ) != 0 ) {
            talloc_free ( connection );
        }
        break;
    case TV_CONNECTION_STATUS_HANDSHAKE_SENDED:
//         if ( wslay_event_want_write ( connection->events ) ) {
//             if ( wslay_event_send ( connection->events ) != 0 ) {
//                 talloc_free ( connection );
//             }
//         }
        break;
    }
}

uint8_t tv_read ( tv_connection * connection, tv_callback callback )
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
        } else if ( count != BUFFER_SIZE ) {
            talloc_buffer_cut ( request, BUFFER_SIZE - count );
            if ( callback != NULL && callback ( connection ) != 0 ) {
                return 4;
            }
            break;
        }
    }

    return 0;
}

uint8_t tv_write ( tv_connection * connection, tv_callback callback, int flags )
{
    flags |= MSG_DONTWAIT;
    
    size_t response_length = connection->response_length;
    ssize_t sended_length  = send ( connection->fd, connection->response_ptr, response_length, flags );
    if ( sended_length == -1 ) {
        if ( errno != EAGAIN && errno != EWOULDBLOCK ) {
            return 1;
        }
    } else {
        response_length -= sended_length;
        if ( response_length == 0 ) {
            talloc_free ( connection->response );
            connection->response        = NULL;
            connection->response_ptr    = NULL;
            connection->response_length = 0;
        }
        connection->response_ptr    += sended_length;
        connection->response_length = response_length;
        if ( callback != NULL && callback ( connection ) != 0 ) {
            return 2;
        }
    }

    return 0;
}