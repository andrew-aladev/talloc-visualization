#include "talloc_events.h"

#include <sys/stat.h>
#include <string.h>

extern inline
uint8_t tv_now ( uint64_t * now );

static const uint8_t talloc_event_size = 5 + sizeof ( uintptr_t ) * 2;

static inline
uint8_t * get_buf ()
{
    uint8_t * buffer = talloc_events->buffer;
    if ( buffer == NULL ) {
        buffer = malloc ( talloc_event_size );
    } else {
        buffer = realloc ( buffer, talloc_events->buffer_size + talloc_event_size );
    }
    if ( buffer == NULL ) {
        return NULL;
    }
    talloc_events->buffer = buffer;
    uint8_t * buf = buffer + talloc_events->buffer_size;
    talloc_events->buffer_size += talloc_event_size;

    return buf;
}

static inline
uint8_t set_data ( uint8_t type, talloc_chunk * address_ptr )
{
    uintptr_t address = ( uintptr_t ) talloc_data_from_chunk ( address_ptr );
    uintptr_t parent;
    if ( address_ptr->parent == NULL ) {
        parent = 0;
    } else {
        parent = ( uintptr_t ) talloc_data_from_chunk ( address_ptr->parent );
    }

    uint64_t now;
    if ( tv_now ( &now ) != 0 ) {
        return 1;
    }

    uint32_t time_diff;
    if ( talloc_events->last_time == 0 ) {
        time_diff = now - talloc_events->start_time;
    } else {
        time_diff = now - talloc_events->last_time;
    }
    talloc_events->last_time = now;

    uint8_t * buf = get_buf ();
    if ( buf == NULL ) {
        return 2;
    }

    uint8_t offset = 0;
    memcpy ( buf + offset, &type, 1 );
    offset++;
    memcpy ( buf + offset, &time_diff, 4 );
    offset += 4;
    memcpy ( buf + offset, &address, sizeof ( uintptr_t ) );
    offset += sizeof ( uintptr_t );
    memcpy ( buf + offset, &parent, sizeof ( uintptr_t ) );

    return 0;
}

uint8_t tv_talloc_on_add ( talloc_chunk * chunk )
{
    if ( talloc_events_lock ) {
        return 0;
    }
    if ( set_data ( TV_TALLOC_EVENT_ADD, chunk ) != 0 ) {
        return 1;
    }
    return 0;
}
uint8_t tv_talloc_on_update ( talloc_chunk * chunk )
{
    if ( talloc_events_lock ) {
        return 0;
    }
    if ( set_data ( TV_TALLOC_EVENT_UPDATE, chunk ) != 0 ) {
        return 1;
    }
    return 0;
}
uint8_t tv_talloc_on_move ( talloc_chunk * chunk )
{
    if ( talloc_events_lock ) {
        return 0;
    }
    if ( set_data ( TV_TALLOC_EVENT_MOVE, chunk ) != 0 ) {
        return 1;
    }
    return 0;
}
uint8_t tv_talloc_on_del ( talloc_chunk * chunk )
{
    if ( talloc_events_lock ) {
        return 0;
    }
    if ( set_data ( TV_TALLOC_EVENT_DELETE, chunk ) != 0 ) {
        return 1;
    }
    return 0;
}

extern inline
tv_talloc_events * tv_talloc_events_new ();

extern inline
void tv_talloc_events_free ( tv_talloc_events * talloc_events );
