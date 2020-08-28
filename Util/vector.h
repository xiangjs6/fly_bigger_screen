//
// Created by xjs on 2020/8/28.
//

#ifndef FLY_BIGGER_SCREEN_VECTOR_H
#define FLY_BIGGER_SCREEN_VECTOR_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
//member
    size_t _nmemb;
    size_t _memb_size;
    void *_begin;
    void *_end;//这个end指向内存结尾
    size_t _len;
    void *_next;

//func
    int (*push_back)(void *);
    void (*pop_back)(void);
    void (*clear)(void);
    int (*insert)(int, void*);
    void (*erase)(int);
    void *(*at)(int);
    void *(*begin)(void);
    void *(*end)(void);
    size_t (*size)(void);
    bool (*empty)(void);
    void *(*extend)(int);
} vector;

int init_vector(vector *p_vector, size_t nmemb, size_t memb_size, void *array);
void destory_vector(vector *p_vector);

#endif //FLY_BIGGER_SCREEN_VECTOR_H
