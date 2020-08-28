//
// Created by xjs on 2020/8/28.
//

#include "vector.h"
#include "Util.h"
#include <memory.h>

#define STEP 1

static int push_back(void *);
static void pop_back(void);
static void clear(void);
static int insert(int, void*);
static void erase(int);
static void *at(int);
static void *begin(void);
static void *end(void);
static size_t size(void);
static bool empty(void);
static void *extend(int);

static const vector vector_template = {
        //member
        ._begin = NULL,
        ._next = NULL,
        ._end = NULL,
        ._len = 0,
        ._nmemb = 0,
        ._memb_size = 0,
        //func
        .push_back = push_back,
        .pop_back = pop_back,
        .clear = clear,
        .insert = insert,
        .erase = erase,
        .at = at,
        .begin = begin,
        .end = end,
        .size = size,
        .empty = empty,
        .extend = extend
};

int init_vector(vector *p_vector, size_t nmemb, size_t memb_size, void *array)
{
    *p_vector = vector_template;
    //member
    int step = (nmemb / STEP + 1) * STEP;
    p_vector->_nmemb = nmemb;
    p_vector->_memb_size = memb_size;
    p_vector->_len = step * memb_size;
    p_vector->_begin = realloc(NULL, p_vector->_len);
    if (!p_vector->_begin)
        return -1;
    memset(p_vector->_begin, 0, p_vector->_len);
    p_vector->_end = p_vector->_begin + p_vector->_len;
    p_vector->_next = p_vector->_begin;

    if (array) {
        char (*src_p)[memb_size] = array, (*dst_p)[memb_size] = p_vector->_next;
        for (int i = 0; i < nmemb; i++)
            memcpy(dst_p++, src_p++, memb_size);
        p_vector->_next = dst_p;
    }
    return 0;
}

void destory_vector(vector *p_vector)
{
    free(p_vector->_begin);
}

static int push_back(void *memb)
{
    vector *this = *pthis();
    if (this->_end == this->_next) {
        this->_len += STEP * this->_memb_size;
        this->_begin = realloc(this->_begin, this->_len);
        if (!this->_begin)
            return -1;
        this->_end = this->_begin + this->_len;
        this->_next = this->_begin + this->_memb_size * this->_nmemb;
    }
    memcpy(this->_next, memb, this->_memb_size);
    this->_next += this->_memb_size;
    this->_nmemb++;
    return 0;
}

static void pop_back(void)
{
    vector *this = *pthis();
    this->_nmemb--;
    this->_next -= this->_memb_size;
}

static void clear(void)
{
    vector *this = *pthis();
    this->_next = this->_begin;
    this->_nmemb = 0;
}

static int insert(int index, void *memb)
{
    vector *this = *pthis();
    if (this->_nmemb < index)
        return -1;
    if (this->_end == this->_next) {
        this->_len += STEP * this->_memb_size;
        this->_begin = realloc(this->_begin, this->_len);
        if (!this->_begin)
            return -1;
        this->_end = this->_begin + this->_len;
        this->_next = this->_begin + this->_memb_size * this->_nmemb;
    }
    void *ptr = this->_begin + index * this->_memb_size;
    memcpy(ptr + this->_memb_size, ptr, (this->_nmemb - index) * this->_memb_size);
    memcpy(ptr, memb, this->_memb_size);
    this->_nmemb++;
    this->_next += this->_memb_size;
    return 0;
}

static void erase(int index)
{
    vector *this = *pthis();
    if (this->_nmemb < index)
        return;
    void *ptr = this->_begin + index * this->_memb_size;
    this->_nmemb--;
    memcpy(ptr, ptr + this->_memb_size, (this->_nmemb - index) * this->_memb_size);
    this->_next -= this->_memb_size;
}

static void *at(int index)
{
    vector *this = *pthis();
    if (this->_nmemb <= index)
        return NULL;
    return this->_begin + index * this->_memb_size;
}

static void *begin(void)
{
    vector *this = *pthis();
    return this->_begin;
}

static void *end(void)
{
    vector *this = *pthis();
    return this->_begin + this->_nmemb * this->_memb_size;
}

static size_t size(void)
{
    vector *this = *pthis();
    return this->_nmemb;
}

static bool empty(void)
{
    vector *this = *pthis();
    return this->_nmemb;
}

static void *extend(int nmemb)
{
    vector *this = *pthis();
    size_t new_size = this->_memb_size * (nmemb + this->_nmemb);
    if (this->_len < new_size) {
        size_t step = (nmemb / STEP + 1) * STEP;
        this->_len += step * this->_memb_size;
        this->_begin = realloc(this->_begin, this->_len);
        if (!this->_begin)
            return NULL;
        this->_end = this->_begin + this->_len;
    }
    void *ptr = this->_begin + this->_nmemb * this->_memb_size;
    this->_nmemb += nmemb;
    this->_next = this->_begin + this->_memb_size * this->_nmemb;
    return ptr;
}