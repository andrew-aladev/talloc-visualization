#include "epoll.h"
#include "socket.h"
#include "process.h"
#include "timer.h"

#include <talloc2/ext/destructor.h>

#include <sys/timerfd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#define MAX_EVENTS SOMAXCONN

uint8_t tv_sockets_wait ( tv_sockets * sockets )
{
    size_t sockets_count = tv_list_get_length ( sockets->socket_list );

    int epoll_fd = epoll_create ( sockets_count );
    if ( epoll_fd == -1 ) {
        return 1;
    }
    sockets->epoll_fd = epoll_fd;

    struct epoll_event * socket_events = talloc ( sockets, sizeof ( struct epoll_event ) * MAX_EVENTS );
    if ( socket_events == NULL ) {
        return 2;
    }
    sockets->events = socket_events;

    tv_list_item * socket_item = sockets->socket_list->last_item;
    tv_socket *    socket;

    int socket_fd;
    struct epoll_event socket_event;
    while ( socket_item != NULL ) {
        socket    = socket_item->data;
        socket_fd = socket->fd;
        if ( listen ( socket_fd, SOMAXCONN ) == -1 ) {
            talloc_free ( socket_events );
            return 3;
        }

        tv_event * event = talloc ( socket, sizeof ( tv_event ) );
        if ( event == NULL ) {
            talloc_free ( socket_events );
            return 4;
        }
        event->type = TV_EVENT_TYPE_SOCKET;
        event->data = socket;

        socket_event.data.ptr = event;
        socket_event.events   = EPOLLIN | EPOLLET;

        if ( epoll_ctl ( epoll_fd, EPOLL_CTL_ADD, socket_fd, &socket_event ) == -1 ) {
            talloc_free ( socket_events );
            return 4;
        }

        socket_item = socket_item->prev;
    }

    if ( tv_timer_wait ( sockets ) != 0 ) {
        talloc_free ( socket_events );
        return 5;
    }

    int ready_events_count, index;
    while ( true ) {
        ready_events_count = epoll_wait ( epoll_fd, socket_events, MAX_EVENTS, -1 );
        if ( errno == EINTR ) {
            talloc_free ( socket_events );
            return 0;
        }
        if ( ready_events_count == -1 ) {
            talloc_free ( socket_events );
            return 5;
        }
        for ( index = 0; index < ready_events_count; index ++ ) {
            socket_event    = socket_events[index];
            uint32_t events = socket_event.events;
            if ( events & EPOLLIN ) {
                tv_event * event = socket_event.data.ptr;
                switch ( event->type ) {
                case TV_EVENT_TYPE_SOCKET:
                    tv_accept ( sockets, event->data );
                    break;
                case TV_EVENT_TYPE_CONNECTION:
                    tv_process ( event->data );
                    break;
                case TV_EVENT_TYPE_TIMER:
                    tv_timer ( sockets );
                    break;
                }
            }
        }
    }

    if ( talloc_free ( socket_events ) != 0 ) {
        return 6;
    }
    return 0;
}

uint8_t tv_connection_wait ( tv_sockets * sockets, tv_connection * connection )
{
    tv_event * event = talloc ( connection, sizeof ( tv_event ) );
    if ( event == NULL ) {
        return 1;
    }
    event->type = TV_EVENT_TYPE_CONNECTION;
    event->data = connection;

    struct epoll_event connection_event;
    connection_event.data.ptr = event;
    connection_event.events   = EPOLLIN | EPOLLOUT | EPOLLET;

    if ( epoll_ctl ( sockets->epoll_fd, EPOLL_CTL_ADD, connection->fd, &connection_event ) == -1 ) {
        talloc_free ( event );
        return 2;
    }
    return 0;
}

uint8_t tv_timer_wait ( tv_sockets * sockets )
{
    struct itimerspec its;
    its.it_interval.tv_sec  = 1;
    its.it_interval.tv_nsec = 0;

    int timer_fd = timerfd_create ( CLOCK_REALTIME, TFD_NONBLOCK );
    if ( timer_fd == -1 ) {
        return 1;
    }
    sockets->timer_fd = timer_fd;
    if ( timerfd_settime ( timer_fd, 0, &its, NULL ) == -1 ) {
        return 2;
    }

    tv_event * event = talloc ( sockets, sizeof ( tv_event ) );
    if ( event == NULL ) {
        return 1;
    }
    event->type = TV_EVENT_TYPE_TIMER;
    event->data = NULL;

    struct epoll_event timer_event;
    timer_event.data.ptr = event;
    timer_event.events   = EPOLLIN | EPOLLET;

    if ( epoll_ctl ( sockets->epoll_fd, EPOLL_CTL_ADD, timer_fd, &timer_event ) == -1 ) {
        talloc_free ( event );
        return 2;
    }
    return 0;
}