#ifndef TALLOC_VISUALIZATION_TALLOC_EVENTS_H
#define TALLOC_VISUALIZATION_TALLOC_EVENTS_H

#include <sys/time.h>
#include <stdbool.h>

#include <talloc2/events.h>

typedef struct tv_talloc_events_t {
    uint64_t start_time;
    uint64_t last_time;
    uint8_t  * buffer;
    size_t   buffer_size;
} tv_talloc_events;

enum {
    TV_TALLOC_EVENT_ADD = 0,
    TV_TALLOC_EVENT_UPDATE,
    TV_TALLOC_EVENT_MOVE,
    TV_TALLOC_EVENT_DELETE
};

tv_talloc_events * talloc_events;
bool talloc_events_lock; // stop creating events while sending events

uint8_t tv_talloc_on_add    ( talloc_chunk * chunk );
uint8_t tv_talloc_on_update ( talloc_chunk * chunk );
uint8_t tv_talloc_on_move   ( talloc_chunk * chunk );
uint8_t tv_talloc_on_del    ( talloc_chunk * chunk );

inline
uint8_t tv_now ( uint64_t * now )
{
    struct timeval tv;
    if ( gettimeofday ( &tv, NULL ) == -1 ) {
        return 1;
    }
    * now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return 0;
}

inline
tv_talloc_events * tv_talloc_events_new ()
{
    talloc_events_lock = false;

    uint64_t now;
    if ( tv_now ( &now ) != 0 ) {
        return NULL;
    }

    tv_talloc_events * talloc_events = malloc ( sizeof ( tv_talloc_events ) );

    talloc_events->start_time  = now;
    talloc_events->last_time   = 0;
    talloc_events->buffer      = NULL;
    talloc_events->buffer_size = 0;

    talloc_set_callback ( tv_talloc_on_add, tv_talloc_on_update, tv_talloc_on_move, tv_talloc_on_del );

    return talloc_events;
}

inline
void tv_talloc_events_free ( tv_talloc_events * talloc_events )
{
    free ( talloc_events->buffer );
    free ( talloc_events );
}

#endif

