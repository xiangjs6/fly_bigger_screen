//
// Created by xjs on 2020/8/11.
//

#ifndef FLY_BIGGER_SCREEN_STACK_H
#define FLY_BIGGER_SCREEN_STACK_H

#include <stdint.h>
#include <stdlib.h>

#define STACK_TOP 0

typedef union {void* p_val; int64_t val;} StackDataType;
typedef struct StackHead StackHead;
typedef struct StackOpts StackOpts;
typedef struct StackNode StackNode;

struct StackOpts {
    void (*copy)(StackDataType *src, StackDataType *dst);
    int (*malloc)(StackDataType *data);
    void (*free)(StackDataType *data);
};

struct StackNode {
    StackDataType data;
    StackNode *next;
};

struct StackHead {
    size_t size;
    StackNode *head;
    StackOpts opts;
};

StackHead initStack(StackOpts opts);
void destoryStack(StackHead *stack);
int putStack(StackHead *stack, StackDataType data);
int popStack(StackHead *stack, StackDataType *data);
StackDataType *getStack(StackHead *stack, int index);
void cleanStack(StackHead *stack);
#endif //FLY_BIGGER_SCREEN_STACK_H
