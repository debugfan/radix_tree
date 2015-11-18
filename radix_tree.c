#include "radix_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define OFFSET_UNIT 1
#define OFFSET_KEY(k, off) k[off]

static char *strndup(const char *s, size_t len)
{
    char *result;

    result = (char *)malloc(len + 1);
    if (!result)
    {
        return NULL;
    }

    result[len] = '\0';
    return (char *)memcpy(result, s, len);
}

static char *mergestr(const char *s1, int s1_len, const char *s2, int s2_len)
{
    char *str;
    int len;

    len = s1_len + s2_len;
    str = malloc(len * sizeof(char));
    if (str)
    {
        memcpy(str, s1, s1_len);
        memcpy(str + s1_len, s2, s2_len);
    }

    return str;
}

radix_tree_node_t *new_radix_tree_node(
    unsigned char key,
    const unsigned char *keys,
    int keys_off,
    int keys_len)
{
    radix_tree_node_t *node;
    int byte_off;
    int bit_off;
    node = (radix_tree_node_t *)calloc(1, sizeof(radix_tree_node_t));
    if (node != NULL)
    {
        node->key = key;
        byte_off = keys_off / OFFSET_UNIT;
        bit_off  =  keys_off % OFFSET_UNIT;
        node->keys = strndup(keys + byte_off, keys_len - byte_off);
        node->keys_off = bit_off;
        node->keys_len = keys_len - byte_off;
        node->next = NULL;
        node->table_items = 0;
        node->table = NULL;
        node->value = NULL;
    }
    return node;
}

void free_radix_tree_node(radix_tree_t *tree, radix_tree_node_t *node)
{
    if (node->table != NULL)
    {
        int table_size = tree == NULL ? 0 : tree->table_size;
        for (int i = 0; i < table_size; i++)
        {
            if (node->table[i] != NULL)
            {
                free_radix_tree_node(tree, node->table[i]);
                node->table[i] = NULL;
                node->table_items--;
            }
        }

        free(node->table);
    }

    if (node->value != NULL && tree->delete_leaf != NULL)
    {
        tree->delete_leaf(node->value);
    }

    if (node->keys != NULL)
    {
        free(node->keys);
    }

    if (node->next != NULL)
    {
        free_radix_tree_node(tree, node->next);
    }

    free(node);
}

void radix_tree_clear_children(radix_tree_t *tree, radix_tree_node_t *node)
{
    if (node->table != NULL)
    {
        for (int i = 0; i < tree->table_size; i++)
        {
            if (node->table[i] != NULL)
            {
                free_radix_tree_node(tree, node->table[i]);
                node->table[i] = NULL;
                node->table_items--;
            }
        }
    }
}

void radix_tree_put_child_node(radix_tree_t *tree,
    radix_tree_node_t *node, 
    unsigned char key, 
    radix_tree_node_t *child)
{
    radix_tree_node_t **p_node;
    int idx;
    int table_size;
    table_size = tree->table_size;
    if (node->table == NULL)
    {
        node->table = malloc(table_size * sizeof(radix_tree_node_t *));
        if (node->table != NULL)
        {
            memset(node->table, 0, table_size * sizeof(radix_tree_node_t *));
        }
    }
    idx = key % tree->table_size;
    for (p_node = &node->table[idx]; *p_node != NULL; p_node = &((*p_node)->next))
    {
        assert(key != node->key);
    }
    *p_node = child;
    node->table_items++;
}

void radix_tree_insert(radix_tree_t *tree,
    const unsigned char *key,
    int key_len,
    void *value)
{
    radix_tree_node_t **p_node;
    radix_tree_node_t *node;
    radix_tree_node_t *new_node;
    int off = 0;
    int a_off = 0;
    int idx;
    int table_size;
    int done_flag = 0;
    table_size = tree->table_size;
    node = tree->root;
    for (off = 0; off < key_len; )
    {
        if (node->table == NULL)
        {
            new_node = new_radix_tree_node(OFFSET_KEY(key, off),
                key,
                off,
                key_len);
            radix_tree_put_child_node(tree, node, OFFSET_KEY(key, off), new_node);
            node = new_node;
            break;
        }

        idx = OFFSET_KEY(key, off) % table_size;
        for (p_node = &node->table[idx]; 
            *p_node != NULL; 
            p_node = &(*p_node)->next)
        {
            if (OFFSET_KEY(key, off) == (*p_node)->key)
            {
                break;
            }
        }

        if (*p_node == NULL)
        {
            *p_node = new_radix_tree_node(OFFSET_KEY(key, off), key, off, key_len);
            node = *p_node;
            break;
        }

        node = *p_node;

        a_off = 0;
        while (off < key_len && a_off + node->keys_off < node->keys_len
            && OFFSET_KEY(key, off) == OFFSET_KEY(node->keys, node->keys_off + a_off))
        {
            off++;
            a_off++;
        }

        if (a_off + node->keys_off < node->keys_len)
        {
            radix_tree_node_t *rest;
            rest = new_radix_tree_node(OFFSET_KEY(node->keys, node->keys_off + a_off),
                node->keys,
                a_off,
                node->keys_len);
            rest->table = node->table;
            rest->table_items = node->table_items;
            rest->value = node->value;
            node->table = NULL;
            node->table_items = 0;
            node->keys_len = a_off;
            node->value = NULL;
            radix_tree_put_child_node(tree, node, OFFSET_KEY(node->keys, node->keys_off + a_off), rest);

            if (off < key_len)
            {
                new_node = new_radix_tree_node(OFFSET_KEY(key, off),
                    key,
                    off,
                    key_len);
                radix_tree_put_child_node(tree, node, OFFSET_KEY(key, off), new_node);
                node = new_node;
            }

            break;
        }
    }

    if (node != NULL)
    {
        if (node->value != NULL && tree->delete_leaf)
        {
            tree->delete_leaf(node->value);
        }

        if (tree->copy_leaf != NULL)
        {
            node->value = tree->copy_leaf(value);
        }
        else
        {
            node->value = value;
        } 
    }
}

void *radix_tree_exact_match(radix_tree_t *tree, const unsigned char *key, int key_len)
{
    radix_tree_node_t *node;
    int off = 0;
    int a_off = 0;
    int idx;
    node = tree->root;
    while (off < key_len)
    {
        if (node->table == NULL)
        {
            break;
        }

        idx = OFFSET_KEY(key, off) % tree->table_size;
        for (node = node->table[idx]; node != NULL; node = node->next)
        {
            if (OFFSET_KEY(key, off) == node->key)
            {
                break;
            }
        }

        if (node == NULL)
        {
            break;
        }

        a_off = 0;
        while (off < key_len && a_off + node->keys_off < node->keys_len
            && OFFSET_KEY(key, off) == OFFSET_KEY(node->keys, node->keys_off + a_off))
        {
            off++;
            a_off++;
        }

        if (a_off + node->keys_off < node->keys_len)
        {
            node = NULL;
            break;
        }
    }

    if (off == key_len && node != NULL && node->value != NULL)
    {
        if (tree->copy_leaf != NULL)
        {
            return tree->copy_leaf(node->value);
        }
        else
        {
            return node->value;
        }
    }
    else
    {
        return NULL;
    }
}

int radix_tree_prefix_match(radix_tree_t *tree, const unsigned char *key, int key_len, void **value)
{
    radix_tree_node_t *node;
    radix_tree_node_t *last;
    int off = 0;
    int a_off = 0;
    int idx;
    int nc = 0;
    node = tree->root;
    last = NULL;
    if (node->value != NULL)
    {
        last = node;
        nc++;
    }
    while (off < key_len)
    {
        if (node->table == NULL)
        {
            break;
        }

        idx = OFFSET_KEY(key, off) % tree->table_size;
        for (node = node->table[idx]; node != NULL; node = node->next)
        {
            if (OFFSET_KEY(key, off) == node->key)
            {
                break;
            }
        }

        if (node == NULL)
        {
            break;
        }

        a_off = 0;
        while (off < key_len && a_off + node->keys_off < node->keys_len
            && OFFSET_KEY(key, off) == OFFSET_KEY(node->keys, node->keys_off + a_off))
        {
            off++;
            a_off++;
        }

        if (a_off + node->keys_off < node->keys_len)
        {
            node = NULL;
            break;
        }

        if (node->value != NULL)
        {
            last = node;
            nc++;
        }
    }

    if (last != NULL && last->value != NULL)
    {
        if (tree->copy_leaf != NULL)
        {
            *value =  tree->copy_leaf(last->value);
        }
        else
        {
            *value = last->value;
        }
    }
    else
    {
        *value = NULL;
    }

    return nc;
}

void radix_tree_merge_node(radix_tree_node_t *node)
{
    radix_tree_node_t *child = NULL;
    char *keys;
    int keys_len;
    while (node->table_items == 1 && node->value != NULL)
    {
        for (int i = 0; i < node->table_items; i++)
        {
            if (node->table[i] != NULL)
            {
                child = node->table[i];
                break;
            }
        }
        assert (child != NULL);
        keys_len = node->keys_len + child->keys_len;
        keys = mergestr(node->keys, node->keys_len, child->keys, child->keys_len);
        node->keys = keys;
        node->keys_len = keys_len;
        free(node->table);
        node->table_items = child->table_items;
        node->table = child->table;
        node->value = child->value;
        child->table_items = 0;
        child->table = NULL;
        child->value = NULL;
        free_radix_tree_node(NULL, child);
    }
}

void radix_tree_unmap_node(radix_tree_t *tree, radix_tree_node_t *parent, radix_tree_node_t *node)
{
    if (node->value != NULL)
    {
        if (tree->delete_leaf != NULL)
        {
            tree->delete_leaf(node->value);
        }
        node->value = NULL;
    }
    if (parent != NULL)
    {
        parent->table_items--;
    }
}

void radix_tree_remove(radix_tree_t *tree, const unsigned char *key, int key_len, void **value)
{
    radix_tree_node_t *parent = NULL;
    radix_tree_node_t **p_node;
    radix_tree_node_t *node;
    int off = 0;
    int a_off = 0;
    int idx;
    p_node = &tree->root;
    node = tree->root;
    parent = NULL;
    while (off < key_len)
    {
        parent = node;

        if (node->table == NULL)
        {
            break;
        }

        idx = OFFSET_KEY(key, off) % tree->table_size;
        for (p_node = &node->table[idx]; 
            (*p_node) != NULL; 
            (*p_node) = (*p_node)->next)
        {
            node = *p_node;
            if (OFFSET_KEY(key, off) == node->key)
            {
                break;
            }
        }

        if (node == NULL)
        {
            break;
        }

        a_off = 0;
        while (off < key_len && a_off + node->keys_off < node->keys_len
            && OFFSET_KEY(key, off) == OFFSET_KEY(node->keys, node->keys_off + a_off))
        {
            off++;
            a_off++;
        }

        if (a_off + node->keys_off < node->keys_len)
        {
            node = NULL;
            break;
        }
    }
    
    if (node != NULL)
    {
        *value = node->value;
        node->value = NULL;

        if (node->table_items == 0)
        {
            (*p_node) = node->next;
            node->next = NULL;
            free_radix_tree_node(tree, node);
            parent->table_items--;
        }
        else
        {
            radix_tree_unmap_node(tree, parent, node);
            if (parent != NULL)
            {
                radix_tree_merge_node(parent);
            }
        }
    }
    else
    {
        *value = NULL;
    }
}

void radix_tree_erase(radix_tree_t *tree, const unsigned char *key, int key_len)
{
    void *value;
    radix_tree_remove(tree, key, key_len, &value);
    if (value != NULL && tree->delete_leaf)
    {
        tree->delete_leaf(value);
    }
}

void radix_tree_clear(radix_tree_t *tree)
{
    radix_tree_clear_children(tree, tree->root);
}

void radix_tree_init(radix_tree_t *tree, 
    int table_size, 
    radix_copy_fn copy_leaf, 
    radix_destruct_fn delete_leaf)
{
    tree->copy_leaf = copy_leaf;
    tree->delete_leaf = delete_leaf;
    tree->table_size = table_size;
    tree->root = new_radix_tree_node(0, NULL, 0, 0);
}

radix_tree_t *radix_tree_create(int table_size, radix_copy_fn copy_leaf, radix_destruct_fn delete_leaf)
{
    radix_tree_t *tree;
    tree = malloc(sizeof(radix_tree_t));
    if (tree != NULL)
    {
        radix_tree_init(tree, table_size, copy_leaf, delete_leaf);
    }
    return tree;
}

void radix_tree_destroy(radix_tree_t *tree)
{
    radix_tree_clear(tree);
    if (tree->root)
    {
        free_radix_tree_node(tree, tree->root);
    }
    free(tree);
}

void radix_tree_dump_node(radix_tree_t *tree, 
    radix_tree_node_t *node, 
    int level)
{
    int i;
    char *keys;

    for (i = 0; i < level; i++) printf("\t");

    keys = strndup(node->keys, node->keys_len);
    printf(" - %s => [%s]\n", keys, node->value ? (char *)node->value : "");
    free(keys);

    if (node->table != NULL)
    {
        for (i = 0; i < tree->table_size; i++)
        {
            if (node->table[i] != NULL)
            {
                radix_tree_dump_node(tree, node->table[i], level + 1);
            }
        }
    }

    node = node->next;
    while (node != NULL)
    {
        radix_tree_dump_node(tree, node, level);
        node = node->next;
    }
}

void radix_tree_dump(radix_tree_t *tree)
{
    radix_tree_dump_node(tree, tree->root, 0);
}
