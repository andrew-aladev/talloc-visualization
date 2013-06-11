#ifndef TALLOC_VISUALIZATION_LIST_H
#define TALLOC_VISUALIZATION_LIST_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <talloc2/tree.h>

typedef struct tv_list_item_t {
    struct tv_list_item_t * prev;
    struct tv_list_item_t * next;
    void * data;
} tv_list_item;

typedef struct tv_list_t {
    struct tv_list_item_t * last_item;
    size_t length;
} tv_list;

inline
tv_list * tv_list_new ( void * ctx )
{
    tv_list * list = talloc ( ctx, sizeof ( tv_list ) );
    if ( list == NULL ) {
        return NULL;
    }

    list->last_item = NULL;
    list->length    = 0;
    return list;
}

inline
uint8_t tv_list_append ( tv_list * list, void * data )
{
    tv_list_item * item = talloc ( list, sizeof ( tv_list_item ) );
    if ( item == NULL ) {
        return 1;
    }
    if ( talloc_move ( data, item ) != 0 ) {
        return 2;
    }
    item->data = data;

    tv_list_item * last_item = list->last_item;
    if ( last_item != NULL ) {
        last_item->next = item;
    }
    item->prev = last_item;
    item->next = NULL;

    list->last_item = item;
    list->length++;
    return 0;
}

inline
void tv_list_delete ( tv_list * list, tv_list_item * item )
{
    tv_list_item * next = item->next;
    tv_list_item * prev = item->prev;
    if ( next != NULL ) {
        next->prev = prev;
    }
    if ( prev != NULL ) {
        prev->next = next;
    }
    list->length --;
}

inline
size_t tv_list_get_length ( tv_list * list )
{
    return list->length;
}

#endif
