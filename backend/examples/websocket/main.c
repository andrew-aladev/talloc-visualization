#include <onion/onion.h>
#include <onion/websocket.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

onion_connection_status root_content ( void * data, onion_websocket * websocket, size_t data_length );

onion_connection_status root_callback ( void * data, onion_request * request, onion_response * response ) {
    onion_websocket * websocket = onion_websocket_new ( request, response );
    if ( !websocket ) {
        return OCS_FORBIDDEN;
    }

    onion_websocket_printf ( websocket, "ololo, pish pish" );
    onion_websocket_set_callback ( websocket, root_content );

    return OCS_WEBSOCKET;
}

onion_connection_status root_content ( void * data, onion_websocket * websocket, size_t data_length ) {
    char * message = malloc ( sizeof ( char ) * ( data_length + 1 ) );
    int length = onion_websocket_read ( websocket, message, data_length );
    if ( !length ) {
        return OCS_INTERNAL_ERROR;
    }
    message[data_length] = '\0';
    printf ( "message : %s\n", message );
    free ( message );
    return OCS_NEED_MORE_DATA;
}

/*
onion_connection_status websocket_example_cont ( void * data, onion_websocket * ws, size_t data_ready_len ) {
    char tmp[256];
    if ( data_ready_len>sizeof ( tmp ) )
        data_ready_len=sizeof ( tmp )-1;

    int len=onion_websocket_read ( ws, tmp, data_ready_len );
    if ( len==0 ) {
        ONION_ERROR ( "Error reading data: %d: %s (%d)", errno, strerror ( errno ), data_ready_len );
        sleep ( 1 );
    }
    tmp[len]=0;
    onion_websocket_printf ( ws, "Echo: %s", tmp );

    ONION_INFO ( "Read from websocket: %d: %s", len, tmp );

    return OCS_NEED_MORE_DATA;
}
*/

static onion * server;

static void stop_listen ( int signal ) {
    if ( server ) {
        onion_listen_stop ( server );
    }
}

int main() {
    server = onion_new ( O_POOL );
    onion_set_max_threads ( server, sysconf ( _SC_NPROCESSORS_ONLN ) );
    //onion_set_certificate ( server, O_SSL_CERTIFICATE_KEY, "certificate.pem", "certificate.key" );

    onion_set_hostname ( server, "0.0.0.0" );
    onion_set_port ( server, "8080" );

    onion_url * router = onion_root_url ( server );
    onion_url_add ( router, "", root_callback );

    signal ( SIGINT,  stop_listen );
    signal ( SIGTERM, stop_listen );
    onion_listen ( server );

    onion_free ( server );
    return 0;
}

