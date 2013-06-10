#include <talloc_visualization/socket.h>
#include <talloc_visualization/epoll.h>

#include <signal.h>

void sigint ( int sig ) {}

int main()
{
    tv_sockets * sockets = tv_sockets_new ( NULL );

    if ( sockets == NULL ) {
        return 1;
    }
    if ( tv_bind ( sockets, "8185" ) != 0 ) {
        talloc_free ( sockets );
        return 2;
    }
    signal ( SIGINT, sigint );
    if ( tv_sockets_wait ( sockets ) != 0 ) {
        talloc_free ( sockets );
        return 3;
    }
    if ( talloc_free ( sockets ) != 0 ) {
        return 4;
    }

    return 0;
}
