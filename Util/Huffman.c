//
// Created by xjs on 2020/8/20.
//

#include "Huffman.h"
#include <memory.h>
#include <string.h>


struct huffman_inner {
    HuffmanData data;
    int pos;
};

static int _cmpfunc(const void* a, const void* b);
static int32_t _index_func(HuffmanData *data, size_t size);

int generateHuffmanCode(HuffmanData *data, size_t size, HuffmanTree *tree, size_t custom_len)
{
    if (size <= 0)
        return -1;
    struct huffman_inner node_arr[size];
    for (int i = 0; i < size; i++) {
        node_arr[i].data = data[i];
        node_arr[i].pos = -1;
    }
    CreateHeap(node_arr, size, sizeof(*node_arr), _cmpfunc);
    int heap_size = size;
    int node_size = heap_size;
    tree->leaf_size = size;

    tree->size = HUFFMAN_NODE_SIZE(node_size);
    int tree_index = 0;

    while (heap_size > 1)
    {
        int index_l = HeapPop(node_arr, heap_size, sizeof(*node_arr), _cmpfunc);
        int pos_l = node_arr[index_l].pos;
        if (pos_l == -1) {
            if (custom_len > 0) {
                tree->tree[tree_index].data.custom = malloc(custom_len);
                memcpy(tree->tree[tree_index].data.custom, node_arr[index_l].data.custom, custom_len);
            } else
                tree->tree[tree_index].data.value = node_arr[index_l].data.value;
            tree->tree[tree_index].data.weight = node_arr[index_l].data.weight;
            tree->tree[tree_index].children[0] = tree->tree[tree_index].children[1] = -1;
            pos_l = node_arr[index_l].pos = tree_index;
            tree_index++;
        }

        int index_r = HeapPop(node_arr, heap_size - 1, sizeof(*node_arr), _cmpfunc);
        int pos_r = node_arr[index_r].pos;
        if (pos_r == -1) {
            if (custom_len > 0) {
                tree->tree[tree_index].data.custom = malloc(custom_len);
                memcpy(tree->tree[tree_index].data.custom, node_arr[index_r].data.custom, custom_len);
            } else
                tree->tree[tree_index].data.value = node_arr[index_r].data.value;
            tree->tree[tree_index].data.weight = node_arr[index_r].data.weight;
            tree->tree[tree_index].children[0] = tree->tree[tree_index].children[1] = -1;
            pos_r = node_arr[index_r].pos = tree_index;
            tree_index++;
        }

        tree->tree[tree_index].children[0] = pos_l;
        tree->tree[pos_l].parent = tree_index;
        tree->tree[tree_index].children[1] = pos_r;
        tree->tree[pos_r].parent = tree_index;
        tree->tree[tree_index].data.weight = tree->tree[pos_l].data.weight + tree->tree[pos_r].data.weight;
        tree->tree[tree_index].data.value = -1;
        node_arr[index_r].data.weight = tree->tree[tree_index].data.weight;
        node_arr[index_r].data.value = -1;
        node_arr[index_r].pos = tree_index;
        SWAP(node_arr[index_r], node_arr[0]);
        AdjustHeap(node_arr, heap_size - 1, sizeof(*node_arr), 0, _cmpfunc);
        tree_index++;

        heap_size--;
    }

    return 0;
}

void HuffmanTreeToCode(HuffmanTree *tree, HuffmanCode *code, size_t custom_len, int32_t(*index_func)(HuffmanData *, size_t))
{
    if (!index_func)
        index_func = _index_func;
    for (int i = 0; i < tree->size; i++) {
        if (tree->tree[i].children[0] == -1 && tree->tree[i].children[1] == -1) {
            int node_index = i;
            HuffmanCode *p_code = &code[index_func(&tree->tree[i].data, tree->leaf_size)];
            if (custom_len > 0)
                memcpy(p_code->custom, tree->tree[i].data.custom, custom_len);
            else
                p_code->value = tree->tree[i].data.value;
            char *ptr = p_code->code;
            while (node_index < tree->size - 1)
            {
                int parent_index = tree->tree[node_index].parent;
                *ptr = tree->tree[parent_index].children[0] == node_index ? '0' : '1';
                ptr++;
                node_index = parent_index;
            }
            *ptr = '\0';
            strrev(p_code->code);
            //code++;
        }
    }
}

int _cmpfunc(const void* a, const void* b)
{
    const struct huffman_inner *_a = a;
    const struct huffman_inner *_b = b;
    return (_b)->data.weight - (_a)->data.weight;
}

static int32_t _index_func(HuffmanData *data, size_t size)
{
    return data->value % size;
}