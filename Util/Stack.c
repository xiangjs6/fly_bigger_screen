//
// Created by xjs on 2020/8/11.
//

#include "Stack.h"

static void _def_copy(StackDataType *src, StackDataType *dst);
static int _def_malloc(StackDataType *data);
static void _def_free(StackDataType *data);

static StackOpts defalut_opts = {.copy = _def_copy, .malloc = _def_malloc, .free = _def_free};
typedef void(*PFunc)(void);

StackHead initStack(StackOpts opts)
{
    StackHead stack_h = {0, NULL, opts};
    PFunc *p_end = (PFunc*)&stack_h.opts + sizeof(StackOpts) / sizeof(PFunc);
    for(PFunc *p_opts = (PFunc*)&stack_h.opts, *p_def_opts = (PFunc*)&defalut_opts; p_opts < p_end; p_opts++, p_def_opts++) {
        if(!*p_opts)
            *p_opts = *p_def_opts;
    }
    return stack_h;
}

void destoryStack(StackHead *stack)
{
    cleanStack(stack);
}

void cleanStack(StackHead *stack)
{
    while (stack->size--)
        popStack(stack, NULL);
}

int putStack(StackHead *stack, StackDataType data)
{
    StackNode *node = malloc(sizeof(StackNode));
    if (!node)
        return -1;
    if (stack->opts.malloc(&node->data) < 0) {
        free(node);
        return -1;
    }
    stack->opts.copy(&data, &node->data);
    node->next = stack->head;
    stack->head = node;
    stack->size++;
    return 0;
}

int popStack(StackHead *stack, StackDataType *data)
{
    if (!stack->size)
        return -1;
    StackNode *node = stack->head;
    if (data)
        stack->opts.copy(&node->data, data);
    stack->opts.free(&node->data);
    stack->head = node->next;
    free(node);
    stack->size--;
    return 0;
}

StackDataType *getStack(StackHead *stack, int index)
{
    if (stack->size <= index)
        return NULL;
    StackNode *node = stack->head;
    while (index--)
        node = node->next;
    return &node->data;
}

static void _def_copy(StackDataType *src, StackDataType *dst)
{
    *dst = *src;
}

static int _def_malloc(StackDataType *data)
{
    return 0;
}

static void _def_free(StackDataType *data)
{

}