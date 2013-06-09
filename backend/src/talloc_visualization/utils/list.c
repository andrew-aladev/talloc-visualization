#include "list.h"

extern inline
tv_list * tv_list_new ( void * ctx );

extern inline
uint8_t tv_list_append ( tv_list * list, void * data );

extern inline
size_t tv_list_get_length ( tv_list * list );