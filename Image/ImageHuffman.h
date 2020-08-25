//
// Created by xjs on 2020/8/25.
//

#ifndef FLY_BIGGER_SCREEN_IMAGEHUFFMAN_H
#define FLY_BIGGER_SCREEN_IMAGEHUFFMAN_H

#include "PImage.h"
int ImageHuffmanEncode(PImage image, HuffmanTree *tree, void *out, size_t *len);
void ImageHuffmanDecode(void *code, HuffmanTree *tree, PImage *out);
#endif //FLY_BIGGER_SCREEN_IMAGEHUFFMAN_H
