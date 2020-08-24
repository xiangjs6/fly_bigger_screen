//
// Created by xjs on 2020/7/17.
//

#ifndef FLY_BIGGER_SCREEN_UTIL_H
#define FLY_BIGGER_SCREEN_UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include "LoopArray.h"
#include "HashMap.h"
#include "ShareMemory.h"
#include "Stack.h"
#include "Heap.h"
#include "Huffman.h"

#define BYTE_ALIGNED  __attribute__((packed, aligned(1)))
#define SWAP(a, b) \
do {\
    typeof(a) _temp = (a);\
    (a) = (b);\
    (b) = _temp;\
} while(0)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
uint16_t checksum(uint16_t *data, size_t len);
char *strrev(char *s);

typedef unsigned char byte;
#endif //FLY_BIGGER_SCREEN_UTIL_H
