//
// Created by xjs on 2020/8/20.
//

#ifndef FLY_BIGGER_SCREEN_HUFFMAN_H
#define FLY_BIGGER_SCREEN_HUFFMAN_H

#include "Util.h"

typedef struct HuffmanData {
    int32_t value;
    int32_t weight;
} HuffmanData;

typedef struct HuffmanCode{
    struct {
        int32_t children[2];
        HuffmanData data;
    } *tree;
    int32_t size;
} HuffmanCode;

//int generateHuffmanCode(void *data, int nmemb, size_t size, HuffmanCode *code, int (*count_func)(void *));
int generateHuffmanCode(HuffmanData *data, size_t size, HuffmanCode *code);
int huffmanEnocde(void *data, size_t len, HuffmanCode *code);
int huffmanDecode(void *data, size_t len, HuffmanCode *code);
void destoryHuffmanCode(HuffmanCode **code);
#endif //FLY_BIGGER_SCREEN_HUFFMAN_H
