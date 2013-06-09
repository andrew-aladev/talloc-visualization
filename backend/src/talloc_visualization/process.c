#include "process.h"
#include "socket.h"

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 512

void tv_read ( tv_connection * connection )
{
    talloc_buffer * buffer = connection->buffer;
    if ( buffer == NULL ) {
        buffer = connection->buffer = talloc_buffer_new ( connection );
    }

    ssize_t count;
    while ( true ) {
        char * buf = talloc_buffer_get ( buffer, BUFFER_SIZE );
        count = read ( connection->fd, buf, BUFFER_SIZE );
        if ( count == -1 ) {
            if ( errno == EAGAIN ) {
                talloc_buffer_cut ( buffer, BUFFER_SIZE );
            } else {
                talloc_free ( connection );
            }
            break;
        } else if ( count != BUFFER_SIZE ) {
            talloc_buffer_cut ( buffer, BUFFER_SIZE - count );
            if ( tv_header ( connection ) != 0 ) {
                talloc_free ( connection );
            }
            break;
        }
    }
}

uint8_t tv_header ( tv_connection * connection )
{
    char * header = connection->buffer->buf;
    char * walk, * key, * value;
    size_t key_size, value_size;

    walk = header;
    while ( walk != NULL ) {
        key = strchr ( walk, ':' );
    }
    printf ( "%s\n", header );

    return 0;
}