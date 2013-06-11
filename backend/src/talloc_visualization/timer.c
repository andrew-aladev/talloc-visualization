#include "timer.h"
#include "talloc_events.h"

static inline
void check_and_send ( tv_connection * connection )
{
    size_t source_buffer_size  = talloc_events->buffer_size;
    size_t current_buffer_size = connection->talloc_events_buffer_length;
    if ( current_buffer_size >= source_buffer_size ) {
        return;
    }

    uint8_t * buf = talloc_events->buffer + current_buffer_size;

    wslay_event_msg message;
    message.opcode     = WSLAY_BINARY_FRAME;
    message.msg        = talloc_events->buffer + current_buffer_size;
    message.msg_length = source_buffer_size - current_buffer_size;
    connection->talloc_events_buffer_length = source_buffer_size;

    wslay_event_context * events = connection->events;
    talloc_events_lock = true;
    wslay_event_queue_msg ( events, &message );
    wslay_event_send ( events );
}

void tv_timer ( tv_sockets * sockets )
{
    int timer_fd = sockets->timer_fd;
    uint64_t timer_value;
    read ( timer_fd, &timer_value, 8 );

    tv_socket * socket;
    tv_connection * connection;
    tv_list_item * socket_item = sockets->socket_list->last_item;
    tv_list_item * connection_item;
    while ( socket_item != NULL ) {
        socket = socket_item->data;
        connection_item = socket->connection_list->last_item;
        while ( connection_item != NULL ) {
            connection = connection_item->data;
            check_and_send ( connection );
            connection_item = connection_item->prev;
        }
        socket_item = socket_item->prev;
    }
}