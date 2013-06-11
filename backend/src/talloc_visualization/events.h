#ifndef TALLOC_VISUALIZATION_EVENTS_H
#define TALLOC_VISUALIZATION_EVENTS_H

#include "socket.h"

uint8_t tv_handshake_sended     ( void * data );
ssize_t tv_event_send_callback  ( wslay_event_context * ctx, const uint8_t * data, size_t len, int flags, void * user_data, bool user_data_sending );
uint8_t tv_talloc_events_sended ( void * data );

#endif
