#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "radix_tree.h"
#include "bit_radix_tree.h"

void assert_radix_tree_remove()
{
    int nc;
    void *leaf;
    radix_tree_t *t = radix_tree_create(128, NULL, NULL);

    radix_tree_insert(t, (unsigned char *)"ab", strlen("ab"), "xy");
    radix_tree_insert(t, (unsigned char *)"abc", strlen("abc"), "xyz");
    radix_tree_remove(t, (unsigned char *)"abc", strlen("abc"), &leaf);
    assert(0 == strcmp((char *)leaf, "xyz"));
    leaf = radix_tree_exact_match(t, (unsigned char *)"ab", strlen("ab"));
    assert(0 == strcmp((char *)leaf, "xy"));
    radix_tree_clear(t);

    radix_tree_insert(t, (unsigned char *)"ab", strlen("ab"), "xy");
    radix_tree_insert(t, (unsigned char *)"abc", strlen("abc"), "xyz");
    radix_tree_remove(t, (unsigned char *)"ab", strlen("ab"), &leaf);
    assert(0 == strcmp((char *)leaf, "xy"));
    leaf = radix_tree_exact_match(t, (unsigned char *)"abc", strlen("abc"));
    assert(0 == strcmp((char *)leaf, "xyz"));
    radix_tree_clear(t);

    radix_tree_insert(t, (unsigned char *)"abcde", strlen("abcde"), "xyzmn");
    radix_tree_insert(t, (unsigned char *)"abcfg", strlen("abcfg"), "xyzop");
    radix_tree_dump(t);
    radix_tree_remove(t, (unsigned char *)"abcde", strlen("abcde"), &leaf);
    radix_tree_dump(t);
    assert(0 == strcmp((char *)leaf, "xyzmn"));
    leaf = radix_tree_exact_match(t, (unsigned char *)"abcfg", strlen("abcfg"));
    assert(0 == strcmp((char *)leaf, "xyzop"));
    radix_tree_clear(t);

    radix_tree_dump(t);
    nc = radix_tree_prefix_match(t, (unsigned char *)"abcfg", strlen("abc"), (void **)&leaf);
    assert(0 == nc);
    assert(NULL == leaf);
    leaf = radix_tree_exact_match(t, (unsigned char *)"abc", strlen("abc"));
    assert(NULL == leaf);

    radix_tree_destroy(t);
}

void assert_radix_tree()
{
    void *leaf;
    int nc;
    radix_tree_t *t = radix_tree_create(1, NULL, NULL);
    radix_tree_insert(t, (unsigned char *)"abc", strlen("abc"), "123");
    radix_tree_insert(t, (unsigned char *)"ab", strlen("ab"), "12");
    radix_tree_insert(t, (unsigned char *)"abcd", strlen("abcd"), "1234");
    radix_tree_insert(t, (unsigned char *)"abef", strlen("abef"), "1256");
    radix_tree_insert(t, (unsigned char *)"abc", strlen("abc"), "000");
    radix_tree_insert(t, (unsigned char *)"", 0, "0000000000");
    radix_tree_insert(t, (unsigned char *)"\0\a\0\b", 4, "0a0b");
    radix_tree_dump(t);
    leaf = radix_tree_exact_match(t, (unsigned char *)"\0\a\0\b", 4);
    assert(0 == strcmp((char *)leaf, "0a0b"));
    nc = radix_tree_prefix_match(t, (unsigned char *)"abcxx", strlen("abcxx"), (void **)&leaf);
    assert(3 == nc);
    assert(0 == strcmp((char *)leaf, "000"));
    leaf = radix_tree_exact_match(t, (unsigned char *)"ab", strlen("ab"));
    assert(0 == strcmp((char *)leaf, "12"));
    leaf = radix_tree_exact_match(t, (unsigned char *)"abcd", strlen("abcd"));
    assert(0 == strcmp((char *)leaf, "1234"));
    leaf = radix_tree_exact_match(t, (unsigned char *)"abef", strlen("abef"));
    assert(0 == strcmp((char *)leaf, "1256"));

    radix_tree_remove(t, (unsigned char *)"abc", strlen("abc"), &leaf);
    assert(0 == strcmp((char *)leaf, "000"));
    radix_tree_remove(t, (unsigned char *)"ab", strlen("ab"), &leaf);
    assert(0 == strcmp((char *)leaf, "12"));

    radix_tree_destroy(t);
}

void assert_bit_radix_tree()
{
    void *leaf;
    int nc;
    bit_radix_tree_t *t = bit_radix_tree_create(128, NULL, NULL);
    bit_radix_tree_insert(t, (unsigned char *)"abc", 8 * strlen("abc"), "123");
    bit_radix_tree_insert(t, (unsigned char *)"ab", 8 * strlen("ab"), "12");
    bit_radix_tree_insert(t, (unsigned char *)"abcd", 8 * strlen("abcd"), "1234");
    bit_radix_tree_insert(t, (unsigned char *)"abef", 8 * strlen("abef"), "1256");
    bit_radix_tree_insert(t, (unsigned char *)"abc", 8 * strlen("abc"), "000");
    bit_radix_tree_insert(t, (unsigned char *)"", 0, "0000000000");
    bit_radix_tree_insert(t, (unsigned char *)"\0\a\0\b", 8 * 4, "0a0b");
    bit_radix_tree_dump(t);
    leaf = bit_radix_tree_exact_match(t, (unsigned char *)"\0\a\0\b", 8 * 4);
    assert(0 == strcmp((char *)leaf, "0a0b"));
    nc = bit_radix_tree_prefix_match(t, (unsigned char *)"abcxx", 8 * strlen("abcxx"), (void **)&leaf);
    assert(3 == nc);
    assert(0 == strcmp((char *)leaf, "000"));
    leaf = bit_radix_tree_exact_match(t, (unsigned char *)"ab", 8 * strlen("ab"));
    assert(0 == strcmp((char *)leaf, "12"));
    leaf = bit_radix_tree_exact_match(t, (unsigned char *)"abcd", 8 * strlen("abcd"));
    assert(0 == strcmp((char *)leaf, "1234"));
    leaf = bit_radix_tree_exact_match(t, (unsigned char *)"abef", 8 * strlen("abef"));
    assert(0 == strcmp((char *)leaf, "1256"));

    bit_radix_tree_remove(t, (unsigned char *)"abc", 8 * strlen("abc"), &leaf);
    assert(0 == strcmp((char *)leaf, "000"));
    bit_radix_tree_remove(t, (unsigned char *)"ab", 8 * strlen("ab"), &leaf);
    assert(0 == strcmp((char *)leaf, "12"));

    bit_radix_tree_destroy(t);
}

int main(int argc, char* argv[])
{
    assert_bit_radix_tree();
    assert_radix_tree();
    assert_radix_tree_remove();
    return 0;
}

