#include "events.h"
#include "process.h"

#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <wslay/context.h>

static inline
uint8_t tv_options ( tv_connection * connection )
{
    struct timeval tv;
    if ( gettimeofday ( &tv, NULL ) == -1 ) {
        return 1;
    }
    uint64_t timestamp = htobe64 ( ( tv.tv_sec ) * 1000 + ( tv.tv_usec ) / 1000 );
    const uint8_t sizeof_ptr = sizeof ( uintptr_t );

    uint8_t msg[9];
    msg[0] = sizeof_ptr;
    memcpy ( msg + 1, &timestamp, 8 );

    wslay_event_msg message;
    message.opcode     = WSLAY_BINARY_FRAME;
    message.msg        = msg;
    message.msg_length = sizeof ( msg );

    wslay_event_context * events = connection->events;
    if (
        wslay_event_queue_msg ( events, &message ) < 0 ||
        wslay_event_send ( events ) < 0
    ) {
        return 2;
    }

    return 0;
}

uint8_t tv_handshake_sended ( tv_connection * connection )
{
    struct wslay_event_callbacks callbacks;
    callbacks.recv_callback     = NULL;
    callbacks.send_callback     = tv_event_send_callback;
    callbacks.genmask_callback  = NULL;

    callbacks.on_frame_recv_start_callback = NULL;
    callbacks.on_frame_recv_chunk_callback = NULL;
    callbacks.on_frame_recv_end_callback   = NULL;
    callbacks.on_msg_recv_callback         = NULL;

    wslay_event_context * events = wslay_server_new ( connection, &callbacks, connection );
    if ( events == NULL ) {
        return 1;
    }
    connection->events = events;
    connection->status = TV_CONNECTION_STATUS_HANDSHAKE_SENDED;

    if ( tv_options ( connection ) != 0 ) {
        return 1;
    }

    return 0;
}

ssize_t tv_event_send_callback ( wslay_event_context * ctx, const uint8_t * data, size_t len, int flags, void * user_data )
{
    tv_connection * connection = user_data;
    uint8_t * response_data    = talloc ( connection, sizeof ( uint8_t ) * len );
    if ( response_data == NULL ) {
        return 1;
    }
    memcpy ( response_data, data, len );
    connection->response        = response_data;
    connection->response_ptr    = response_data;
    connection->response_length = len;

    int send_flags = 0;
    if ( ( flags & WSLAY_MSG_MORE ) != 0 ) {
        send_flags |= MSG_MORE;
    }

    if ( tv_write ( connection, NULL, send_flags ) != 0 ) {
        wslay_event_set_error ( ctx, WSLAY_ERR_CALLBACK_FAILURE );
        talloc_free ( connection );
        return -1;
    } else if ( errno == EAGAIN || errno == EWOULDBLOCK ) {
        wslay_event_set_error ( ctx, WSLAY_ERR_WOULDBLOCK );
        return -1;
    }
    return len;
}