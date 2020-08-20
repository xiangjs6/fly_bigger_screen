//
// Created by xjs on 2020/8/20.
//

#ifndef FLY_BIGGER_SCREEN_HUFFMAN_H
#define FLY_BIGGER_SCREEN_HUFFMAN_H

#include "Util.h"

typedef struct HuffmanCode{
    int *tree;
    int size;
    int root;
} HuffmanCode;

int huffmanEnocde(void *data, int nmemb, size_t size, HuffmanCode *code, int (*count_func)(void *));
int huffmanDecode(void *data, size_t len, HuffmanCode *code);
void destoryHuffmanCode(HuffmanCode **code);
#endif //FLY_BIGGER_SCREEN_HUFFMAN_H
