#include <talloc_visualization/socket.h>
#include <talloc_visualization/epoll.h>
#include <talloc_visualization/talloc_events.h>

#include <signal.h>

void sigint ( int sig ) {}

int main()
{
    talloc_events = tv_talloc_events_new ();
    if ( talloc_events == NULL ) {
        return 1;
    }

    tv_sockets * sockets = tv_sockets_new ( NULL );

    if ( sockets == NULL ) {
        tv_talloc_events_free ( talloc_events );
        return 2;
    }
    if ( tv_bind ( sockets, "8181" ) != 0 ) {
        talloc_free ( sockets );
        tv_talloc_events_free ( talloc_events );
        return 3;
    }
    signal ( SIGINT, sigint );
    if ( tv_sockets_wait ( sockets ) != 0 ) {
        talloc_free ( sockets );
        tv_talloc_events_free ( talloc_events );
        return 4;
    }
    if ( talloc_free ( sockets ) != 0 ) {
        tv_talloc_events_free ( talloc_events );
        return 5;
    }
    tv_talloc_events_free ( talloc_events );

    return 0;
}

