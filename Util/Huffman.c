//
// Created by xjs on 2020/8/20.
//

#include "Huffman.h"

#define HASH_SIZE 1024

struct huffman_inner {
    HuffmanData data;
    int pos;
};

static int _def_count(void *ptr);
static int _cmpfunc (const void* a, const void* b);

int generateHuffmanCode(HuffmanData *data, size_t size, HuffmanCode *code)
{
    if (size <= 0)
        return -1;
    /*if (nmemb < 0 || size < 0)
        return -1;
    HashMap map = creatHashMap(HASH_SIZE, (HashDataOpts){NULL});
    struct hash_val *node_arr = calloc(nmemb, sizeof(struct hash_val));
    //HashNode *node_arr[nmemb];
    //struct hash_val node_arr[14] = {};
    if (!node_arr)
        return -1;
    int node_size = 0;
    char *ptr = data;
    if (!count_func)
        count_func = _def_count;
    for (int i = 0; i < nmemb; i++) {
        int32_t value;
        HashNode *node;
        value = count_func(ptr);
        switch (addressHashMap(&map, (HashKey){.key = value}, &node))
        {
            case 0:
                break;
            case 1:
                node_arr[node_size].pos = -1;
                node_arr[node_size++].node = node;
                break;
            case -1:
                goto err;
        }
        node->data.val.val++;
        ptr += size;
    }*/
    //node_arr = realloc(node_size, sizeof(HashNode*));
    //struct huffman_inner *node_arr = calloc(size, sizeof(struct huffman_inner));
    struct huffman_inner node_arr[7];
    for (int i = 0; i < size; i++) {
        node_arr[i].data = data[i];
        node_arr[i].pos = -1;
    }
    CreateHeap(node_arr, size, sizeof(*node_arr), _cmpfunc);
    int heap_size = size;
    int node_size = heap_size;

    code->tree = calloc(node_size * 2 - 1, sizeof(*code->tree));
    code->size = node_size * 2 - 1;
    int tree_index = 0;
    if (!code->tree)
        goto err;

    while (heap_size > 1)
    {
        int index_l = HeapPop(node_arr, heap_size, sizeof(*node_arr), _cmpfunc);
        int pos_l = node_arr[index_l].pos;
        if (pos_l == -1) {
            code->tree[tree_index].data.value = node_arr[index_l].data.value;
            code->tree[tree_index].data.weight = node_arr[index_l].data.weight;
            code->tree[tree_index].children[0] = code->tree[tree_index].children[1] = -1;
            pos_l = node_arr[index_l].pos = tree_index;
            tree_index++;
        }

        int index_r = HeapPop(node_arr, heap_size - 1, sizeof(*node_arr), _cmpfunc);
        int pos_r = node_arr[index_r].pos;
        if (pos_r == -1) {
            code->tree[tree_index].data.value = node_arr[index_r].data.value;
            code->tree[tree_index].data.weight = node_arr[index_r].data.weight;
            code->tree[tree_index].children[0] = code->tree[tree_index].children[1] = -1;
            pos_r = node_arr[index_r].pos = tree_index;
            tree_index++;
        }

        code->tree[tree_index].children[0] = pos_l;
        code->tree[tree_index].children[1] = pos_r;
        code->tree[tree_index].data.weight = code->tree[pos_l].data.weight + code->tree[pos_r].data.weight;
        code->tree[tree_index].data.value = -1;
        node_arr[index_r].data.weight = code->tree[tree_index].data.weight;
        node_arr[index_r].data.value = -1;
        node_arr[index_r].pos = tree_index;
        SWAP(node_arr[index_r], node_arr[0]);
        AdjustHeap(node_arr, heap_size - 1, sizeof(*node_arr), 0, _cmpfunc);
        tree_index++;

        heap_size--;
    }

    //free(node_arr);
    return 0;
    err:
    free(node_arr);
    free(code->tree);
    return -1;
}

static int _def_count(void *ptr)
{
    return *(int32_t *)ptr;
}

int _cmpfunc (const void* a, const void* b)
{
    struct huffman_inner *_a = a;
    struct huffman_inner *_b = b;
    return (_b)->data.weight - (_a)->data.weight;
}