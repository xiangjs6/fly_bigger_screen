//
// Created by xjs on 2020/7/20.
//

#include "LoopArray.h"
#include <stdarg.h>

static void _def_copy(LoopArrayDataType *src, LoopArrayDataType *dst);
static int _def_malloc(LoopArrayDataType *data);
static void _def_free(LoopArrayDataType *data);

static LoopArrayOpts defalut_opts = {.copy = _def_copy, .malloc = _def_malloc, .free = _def_free};
typedef void(*PFunc)(void);

LoopArrayHead creatLoopArray(int size, LoopArrayOpts opts, void *out_mem_ptr)
{
    LoopArrayHead L_array = {.index = 0, .size = size, .opts = opts};
    L_array.array = out_mem_ptr ? out_mem_ptr : (LoopArrayDataType*)malloc(sizeof(LoopArrayDataType) * size);
    L_array.memory_from_out = out_mem_ptr;
    if (!L_array.array)
        return L_array;
    PFunc *p_end = (PFunc*)&L_array.opts + sizeof(LoopArrayOpts) / sizeof(PFunc);
    for(PFunc *p_opts = (PFunc*)&L_array.opts, *p_def_opts = (PFunc*)&defalut_opts; p_opts < p_end; p_opts++, p_def_opts++) {
        if(!*p_opts)
            *p_opts = *p_def_opts;
    }
    for(int i = 0; i < size; i++) {
        if (L_array.opts.malloc(&L_array.array[i]) < 0) {
            for (int j = 0; j < i; j++)
                L_array.opts.free(&L_array.array[j]);
            free(L_array.array);
            L_array.array = NULL;
            L_array.size = 0;
            break;
        }
    }
    return L_array;
}

void destoryLoopArray(LoopArrayHead *L_array)
{
    for(int i = 0; i < L_array->size; i++)
        L_array->opts.free(&L_array->array[i]);
    if (!L_array->memory_from_out)
        free(L_array->array);
}

int pushLoopArray(LoopArrayHead *L_array, LoopArrayDataType data)
{
    L_array->opts.copy(&L_array->array[L_array->index], &data);
    int old_index = L_array->index;
    L_array->index = (L_array->index + 1) % L_array->size;
    return old_index;
}

LoopArrayDataType getLoopArray(LoopArrayHead *L_array, int index)
{
    return L_array->array[index];
}

static void _def_copy(LoopArrayDataType *src, LoopArrayDataType *dst) {*dst = *src;}
static int _def_malloc(LoopArrayDataType *data) {return 0;}
static void _def_free(LoopArrayDataType *data) {}