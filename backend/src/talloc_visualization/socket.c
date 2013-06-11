#include "socket.h"
#include "epoll.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#include <talloc2/helpers.h>

uint8_t tv_bind ( tv_sockets * sockets, const char * port )
{
    struct addrinfo hints;
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;
    hints.ai_flags     = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_protocol  = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_addrlen   = 0;
    hints.ai_next      = NULL;

    struct addrinfo * addrs;
    if ( getaddrinfo ( NULL, port, &hints, &addrs ) != 0 ) {
        return 1;
    }

    int yes = 1;
    int socket_fd;
    int socket_fd_flags;
    tv_socket * socket_ptr;
    struct addrinfo * addr = addrs;

    while ( addr != NULL ) {
        socket_fd = socket ( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
        if ( socket_fd == -1 ) {
            freeaddrinfo ( addrs );
            return 2;
        }

        socket_ptr = tv_socket_new ( sockets->socket_list );
        if ( socket_ptr == NULL ) {
            freeaddrinfo ( addrs );
            return 3;
        }
        socket_ptr->fd = socket_fd;

        socket_fd_flags = fcntl ( socket_fd, F_GETFL, 0 );
        if (
            socket_fd_flags == -1 ||
            fcntl ( socket_fd, F_SETFL, socket_fd_flags | O_NONBLOCK ) == -1 ||
            setsockopt ( socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof ( yes ) ) == -1
        ) {
            talloc_free ( socket_ptr );
            freeaddrinfo ( addrs );
            return 5;
        }

        // by default ipv6 address will try to listen on both ipv4 and ipv6 address
        if ( addr->ai_family == AF_INET6 ) {
            if ( setsockopt ( socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof ( yes ) ) == -1 ) {
                talloc_free ( socket_ptr );
                freeaddrinfo ( addrs );
                return 6;
            }
        }

        if ( bind ( socket_fd, addr->ai_addr, addr->ai_addrlen ) == -1 ) {
            talloc_free ( socket_ptr );
            freeaddrinfo ( addrs );
            return 7;
        }

        if ( tv_list_append ( sockets->socket_list, socket_ptr ) != 0 ) {
            talloc_free ( socket_ptr );
            freeaddrinfo ( addrs );
            return 8;
        }

        addr = addr->ai_next;
    }

    freeaddrinfo ( addrs );
    return 0;
}

void tv_accept ( tv_sockets * sockets, tv_socket * socket )
{
    struct sockaddr in_addr;
    socklen_t in_len = sizeof ( in_addr );
    int connection_fd = accept ( socket->fd, &in_addr, &in_len );
    if ( connection_fd == -1 ) {
        return;
    }
    tv_connection * connection = tv_connection_new ( socket->connection_list );
    if ( connection == NULL ) {
        return;
    }
    connection->fd = connection_fd;
    if ( tv_list_append ( socket->connection_list, connection ) != 0 ) {
        talloc_free ( connection );
        return;
    }

    int connection_fd_flags = fcntl ( connection_fd, F_GETFL, 0 );
    if ( fcntl ( connection_fd, F_SETFL, connection_fd_flags | O_NONBLOCK ) == -1 ) {
        talloc_free ( connection );
        return;
    }

    if ( tv_connection_wait ( sockets, connection ) != 0 ) {
        talloc_free ( connection );
        return;
    }
}

extern inline
tv_sockets * tv_sockets_new ( void * ctx );

extern inline
uint8_t tv_sockets_close ( void * data );

extern inline
tv_socket * tv_socket_new ( void * ctx );

extern inline
uint8_t tv_socket_close ( void * data );

extern inline
tv_connection * tv_connection_new ( tv_list * connection_list );

extern inline
uint8_t tv_connection_close ( void * data );