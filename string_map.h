#ifndef STRING_MAP_H
#define STRING_MAP_H

#include "radix_tree.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef radix_tree_t string_map_t;

#ifndef STRING_MAP_BOOL
#define STRING_MAP_BOOL     int
#define STRING_MAP_TRUE     1
#define STRING_MAP_FALSE    0
#endif

static __inline void string_map_init(string_map_t *map, 
    radix_copy_fn copy_func, 
    radix_destruct_fn destruct_func)
{
    radix_tree_init(map, 
        copy_func, 
        destruct_func);
}

static __inline  void string_map_insert(string_map_t *map, 
    const unsigned char *key, 
    int key_len, 
    void *value)
{
    radix_tree_insert(map, key, key_len, value);
}

static __inline  void string_map_erase(string_map_t *map, 
    const unsigned char *key, 
    int key_len)
{
    radix_tree_erase(map, key, key_len);
}

static __inline STRING_MAP_BOOL string_map_exists(string_map_t *map, 
    const unsigned char *key, 
    int key_len)
{
    return NULL == radix_tree_exact_match(map, key, key_len);
}

static __inline void string_map_clear(string_map_t *map)
{
    radix_tree_clear(map);
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
