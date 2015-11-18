#ifndef STRING_MAP_H
#define STRING_MAP_H

#include "rx_tree.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef rx_tree string_map_t;

#ifndef STRING_MAP_BOOL
#define STRING_MAP_BOOL     int
#define STRING_MAP_TRUE     1
#define STRING_MAP_FALSE    0
#endif

static __inline void string_map_init(string_map_t *map, rx_copy_fn copy_func, rx_destruct_fn destruct_func)
{
    rx_tree_init(map, copy_func, destruct_func);
}

static __inline  void string_map_insert(string_map_t *map, const unsigned char *key, int key_len, void *value)
{
    rx_tree_add(map, key, key_len, value);
}

static __inline  void string_map_erase(string_map_t *map, const unsigned char *key, int key_len)
{
    rx_tree_erase(map, key, key_len);
}

static __inline STRING_MAP_BOOL string_map_exists(string_map_t *map, const unsigned char *key, int key_len)
{
    return NULL == rx_tree_exact_match(map, key, key_len);
}

static __inline void string_map_clear(string_map_t *map)
{
    rx_tree_clear(map);
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
