#ifndef mdict_simple_key_item_h
#define mdict_simple_key_item_h

#include <stdint.h>

typedef struct simple_key_list_item {
    uint64_t record_start; // Supports files >4GB
    char* key_word;
} simple_key_item;


#endif
