//
// Created by xjs on 2020/7/28.
//

#ifndef FLY_BIGGER_SCREEN_SHAREMEMORY_H
#define FLY_BIGGER_SCREEN_SHAREMEMORY_H
#include <stdlib.h>
#include <stdint.h>

#define ANONYMOUS_KEY 1
#define AUTO_KEY 2

int initShareMemory(size_t size);
void destroyShareMemory(void);
void *shareMalloc(size_t size, uint32_t key);
void shareFree(void *ptr);
void *getShareMemory(uint32_t key);
uint32_t getShareKey(void *ptr);
#endif //FLY_BIGGER_SCREEN_SHAREMEMORY_H
