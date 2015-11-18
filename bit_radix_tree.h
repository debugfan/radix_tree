#ifndef BIT_RADIX_TREE_H
#define BIT_RADIX_TREE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _bit_radix_tree_node
{
    struct _bit_radix_tree_node *next; //sibling
    unsigned char key;
    unsigned char *keys;
    int keys_off;
    int keys_len;
    void *value;
    // children
    int table_items;
    struct _bit_radix_tree_node **table;
} bit_radix_tree_node_t;

typedef void *(*bit_radix_copy_fn)(void *);
typedef void (*bit_radix_destruct_fn)(void *);

typedef struct
{
    bit_radix_tree_node_t *root;
    int table_size;
    bit_radix_copy_fn copy_leaf;
    bit_radix_destruct_fn delete_leaf;
} bit_radix_tree_t;

bit_radix_tree_t *bit_radix_tree_create(int table_size, bit_radix_copy_fn copy_leaf, bit_radix_destruct_fn delete_leaf);
void bit_radix_tree_init(bit_radix_tree_t *tree, int table_size, bit_radix_copy_fn copy_leaf, bit_radix_destruct_fn delete_leaf);
void bit_radix_tree_insert(bit_radix_tree_t *tree, const unsigned char *key, int key_len, void *value);
void *bit_radix_tree_exact_match(bit_radix_tree_t *tree, const unsigned char *key, int key_len);
int bit_radix_tree_prefix_match(bit_radix_tree_t *tree, const unsigned char *key, int key_len, void **value);
void bit_radix_tree_remove(bit_radix_tree_t *tree, const unsigned char *key, int key_len, void **value);
void bit_radix_tree_erase(bit_radix_tree_t *tree, const unsigned char *key, int key_len);
void bit_radix_tree_clear(bit_radix_tree_t *tree);
void bit_radix_tree_destroy(bit_radix_tree_t *tree);
void bit_radix_tree_dump(bit_radix_tree_t *tree);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
