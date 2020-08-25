//
// Created by xjs on 2020/8/20.
//

#ifndef FLY_BIGGER_SCREEN_HUFFMAN_H
#define FLY_BIGGER_SCREEN_HUFFMAN_H

#include "Util.h"

#define HUFFMAN_NODE_SIZE(n) (((n) * 2) - 1)
#define CODE_LENGTH 32

typedef struct HuffmanData {
    union {
        int32_t value;
        void *custom;
    };
    int32_t weight;
} HuffmanData;

typedef struct HuffmanNode {
    int32_t children[2];
    int32_t parent;
    HuffmanData data;
} HuffmanNode;

typedef struct HuffmanTree {
    HuffmanNode *tree;
    int32_t size;
    int32_t leaf_size;
} HuffmanTree;

typedef struct HuffmanCode {
    union {
        int32_t value;
        void *custom;
    };
    char code[CODE_LENGTH];
} HuffmanCode;

//int generateHuffmanCode(void *data, int nmemb, size_t size, HuffmanCode *code, int (*count_func)(void *));
void generateHuffmanTree(HuffmanData *data, size_t size, HuffmanTree *tree, size_t custom_len);
void HuffmanTreeToCode(HuffmanTree *tree, void *code, size_t custom_len, HuffmanCode *(*index_func)(HuffmanData *, void *, size_t));
#endif //FLY_BIGGER_SCREEN_HUFFMAN_H
