//
// Created by xjs on 2020/8/20.
//

#include "Huffman.h"

#define HASH_SIZE 1024

struct hash_val {

};

static int _def_count(void *ptr);
static int _cmpfunc (const void* a, const void* b);

int huffmanEnocde(void *data, int nmemb, size_t size, HuffmanCode *code, int (*count_func)(void *))
{
    HashMap map = creatHashMap(HASH_SIZE, (HashDataOpts){NULL});
    char *ptr = data;
    if (!count_func)
        count_func = _def_count;
    for (int i = 0; i < nmemb; i++) {
        int32_t value;
        HashNode *node;
        value = count_func(ptr);
        if(addressHashMap(&map, (HashKey){.key = value}, &node) < 0)
            goto err;
        node->data.val.val++;
        ptr += size;
    }
    qsort(data, nmemb, size, _cmpfunc);

    return 0;
    err:
    destoryHashMap(&map);
    return -1;
}

static int _def_count(void *ptr)
{
    return *(int32_t *)ptr;
}

int _cmpfunc (const void* a, const void* b)
{
    return (*(int*)b - *(int*)a);
}