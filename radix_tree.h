#ifndef RADIX_TREE_H
#define RADIX_TREE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _radix_tree_node
{
    struct _radix_tree_node *next; //sibling
    unsigned char key;
    unsigned char *keys;
    int keys_off;
    int keys_len;
    void *value;
    // children
    int table_items;
    struct _radix_tree_node **table;
} radix_tree_node_t;

typedef void *(*radix_copy_fn)(void *);
typedef void (*radix_destruct_fn)(void *);

typedef struct
{
    radix_tree_node_t *root;
    int table_size;
    radix_copy_fn copy_leaf;
    radix_destruct_fn delete_leaf;
} radix_tree_t;

radix_tree_t *radix_tree_create(int table_size, radix_copy_fn copy_leaf, radix_destruct_fn delete_leaf);
void radix_tree_init(radix_tree_t *tree, int table_size, radix_copy_fn copy_leaf, radix_destruct_fn delete_leaf);
void radix_tree_insert(radix_tree_t *tree, const unsigned char *key, int key_len, void *value);
void *radix_tree_exact_match(radix_tree_t *tree, const unsigned char *key, int key_len);
int radix_tree_prefix_match(radix_tree_t *tree, const unsigned char *key, int key_len, void **value);
void radix_tree_remove(radix_tree_t *tree, const unsigned char *key, int key_len, void **value);
void radix_tree_clear(radix_tree_t *tree);
void radix_tree_destroy(radix_tree_t *tree);
void radix_tree_dump(radix_tree_t *tree);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
