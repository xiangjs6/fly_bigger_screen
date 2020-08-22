//
// Created by xjs on 2020/8/21.
//

#include "Heap.h"
#include <memory.h>

void AdjustHeap(void *data, int nmemb, int size, int location, int (*cmp_func)(const void *, const void *))
{
    char (*ptr)[size] = data;
    char key[size];
    memcpy(key, ptr + location, size);
    for (int i = 2 * location + 1; i < nmemb; i = i * 2 + 1) {
        if (i + 1 < nmemb && cmp_func(ptr + i, ptr + i + 1) < 0) //找两者中最大的值
            i++;
        if (cmp_func(key, ptr + i) >= 0)//确定location的位置为i
            break;
        memcpy(ptr + location, ptr + i, size);
        location = i;
    }
    memcpy(ptr + location, key, size);
}

void CreateHeap(void *data, int nmemb, int size, int (*cmp_func)(const void *, const void *))
{
    for (int i = nmemb / 2 - 1; i >= 0; i--) {
        AdjustHeap(data, nmemb, size, i, cmp_func);
    }
}

void HeapSort(void *data, int nmemb, int size, int (*cmp_func)(const void *, const void *))
{
    char (*ptr)[size] = data;
    for (int i = nmemb - 1; i >= 0; i--) {
        HeapPop(data, i - 1, size, cmp_func);
/*
        char _temp[size];
        memcpy(_temp, ptr[i], size);
        memcpy(ptr[i], *ptr, size);
        memcpy(*ptr, _temp, size);

        AdjustHeap(data, i - 1, size, 0, cmp_func);
*/
    }
}

int HeapPop(void *data, int nmemb, int size, int (*cmp_func)(const void *, const void *))
{
    char (*ptr)[size] = data;
    char _temp[size];
    int res = nmemb - 1;
    memcpy(_temp, ptr[res], size);
    memcpy(ptr[res], *ptr, size);
    memcpy(*ptr, _temp, size);
    AdjustHeap(data, nmemb - 1, size, 0, cmp_func);
    return res;
}