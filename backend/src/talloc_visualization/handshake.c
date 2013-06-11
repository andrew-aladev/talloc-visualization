#include "handshake.h"
#include "process.h"
#include "events.h"

#include <libbtr/btih.h>
#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>

static inline
uint8_t tv_parse_header ( tv_connection * connection )
{
    char * header = connection->request->buf;
    char * delim, * key, * value, * end = header;
    size_t key_size, value_size;

    delim = header;
    while ( ( delim = strchr ( end, ':' ) ) != NULL ) {
        key = strchr ( end, '\n' );
        if ( key == NULL ) {
            return 1;
        }
        key ++;
        key_size = delim - key;

        delim++;
        while ( * delim == ' ' ) {
            delim++;
        }
        value = delim;
        end = strchr ( value, '\r' );
        if ( end == NULL ) {
            return 2;
        }
        value_size = end - value;
        delim      = end;

        if ( strncmp ( key, "Upgrade", key_size ) == 0 ) {
            connection->upgrade = value;
            connection->upgrade_length = value_size;
        } else if ( strncmp ( key, "Sec-WebSocket-Key", key_size ) == 0 ) {
            connection->key = value;
            connection->key_length = value_size;
        } else if ( strncmp ( key, "Sec-WebSocket-Version", key_size ) == 0 ) {
            connection->version = value;
            connection->version_length = value_size;
        }
    }

    return 0;
}

static inline
uint8_t tv_header_response ( tv_connection * connection, const char * answer, size_t length )
{
    const char header_start[] =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: ";
    size_t header_start_length = sizeof ( header_start ) - 1;
    const char header_end[] = "\r\n\r\n";
    size_t header_end_length = sizeof ( header_end ) - 1;

    size_t response_length = header_start_length + header_end_length + length;
    char * response = talloc ( connection, sizeof ( char ) * ( response_length ) );
    if ( response == NULL ) {
        return 1;
    }
    connection->response        = response;
    connection->response_ptr    = response;
    connection->response_length = response_length;
    strncpy ( response, header_start, header_start_length );
    strncpy ( response + header_start_length, answer, length );
    strncpy ( response + header_start_length + length, header_end, header_end_length );

    connection->status         = TV_CONNECTION_STATUS_HANDSHAKE_SENDING;
    connection->write_callback = tv_handshake_sended;

    tv_process ( connection );

    return 0;
}

static const char *  magic        = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static const uint8_t magic_length = 36;

uint8_t tv_handshake ( void * data )
{
    tv_connection * connection = data;
    
    if ( tv_parse_header ( connection ) != 0 ) {
        return 1;
    }
    char * upgrade = connection->upgrade;
    if ( upgrade == NULL || strncmp ( upgrade, "websocket", connection->upgrade_length ) != 0 ) {
        return 2;
    }
    char * version = connection->version;
    if ( version == NULL || strncmp ( version, "13", connection->version_length ) != 0 ) {
        return 3;
    }

    char * key = connection->key;
    if ( key == NULL ) {
        return 4;
    }

    size_t key_length     = connection->key_length;
    size_t control_length = key_length + magic_length;
    char * control = talloc ( connection, sizeof ( char ) * ( control_length ) );
    if ( control == NULL ) {
        return 5;
    }
    strncpy ( control, key, key_length );
    strncpy ( control + key_length, magic, magic_length );

    bt_hash * hash = talloc ( connection, sizeof ( bt_hash ) );
    if ( hash == NULL ) {
        return 6;
    }
    hash->length = 20;
    uint8_t * binary = talloc ( hash, sizeof ( uint8_t ) * 20 );
    if ( binary == NULL ) {
        return 7;
    }
    hash->binary = binary;

    if ( gnutls_hash_fast ( GNUTLS_DIG_SHA1, control, control_length, binary ) < 0 ) {
        talloc_free ( hash );
        talloc_free ( control );
        return 8;
    }
    if ( talloc_free ( control ) != 0 ) {
        talloc_free ( hash );
        return 9;
    }

    size_t answer_length;
    char * answer = bt_base64_encode ( hash, hash, &answer_length );
    if ( answer == NULL ) {
        talloc_free ( hash );
        return 10;
    }

    if ( tv_header_response ( connection, answer, answer_length ) != 0 ) {
        talloc_free ( hash );
        return 11;
    }

    if ( talloc_free ( hash ) != 0 ) {
        return 12;
    }

    return 0;
}
