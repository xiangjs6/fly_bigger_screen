//
// Created by xjs on 2020/8/21.
//

#ifndef FLY_BIGGER_SCREEN_HEAP_H
#define FLY_BIGGER_SCREEN_HEAP_H

void AdjustHeap(void *data, int nmemb, int size, int location, int (*cmp_func)(const void *, const void *));
void CreateHeap(void *data, int nmemb, int size, int (*cmp_func)(const void *, const void *));
void HeapSort(void *data, int nmemb, int size, int (*cmp_func)(const void *, const void *));
int HeapPop(void *data, int nmemb, int size, int (*cmp_func)(const void *, const void *));
#endif //FLY_BIGGER_SCREEN_HEAP_H
