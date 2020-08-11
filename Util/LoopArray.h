//
// Created by xjs on 2020/7/20.
//

#ifndef FLY_BIGGER_SCREEN_LOOPARRAY_H
#define FLY_BIGGER_SCREEN_LOOPARRAY_H
#include <stdlib.h>
#include <stdbool.h>
typedef union {void* p_val; int64_t val;} LoopArrayDataType;
typedef struct LoopArrayHead LoopArrayHead;
typedef struct LoopArrayOpts LoopArrayOpts;

struct LoopArrayOpts {
    void (*copy)(LoopArrayDataType *src, LoopArrayDataType *dst);
    int (*malloc)(LoopArrayDataType *data);
    void (*free)(LoopArrayDataType *data);
};

struct LoopArrayHead {
    unsigned int index;
    size_t size;
    LoopArrayDataType *array;
    LoopArrayOpts opts;
    bool memory_from_out;
};

LoopArrayHead creatLoopArray(int size, LoopArrayOpts opts, void *out_mem_ptr);
void destoryLoopArray(LoopArrayHead *L_array);
int pushLoopArray(LoopArrayHead *L_array, LoopArrayDataType data);
LoopArrayDataType getLoopArray(LoopArrayHead *L_array, int index);

#endif //FLY_BIGGER_SCREEN_LOOPARRAY_H
