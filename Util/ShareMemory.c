//
// Created by xjs on 2020/7/28.
//

#include "ShareMemory.h"
#include <pthread.h>
#include <sys/mman.h>

struct MemInfo {
    size_t size;
    uint32_t key;
    void *base;
};

struct MemNode {
    struct MemInfo info;
    struct MemNode *pre;
    struct MemNode *next;
};

static struct {
    size_t size;
    pthread_mutex_t lock;
    uint32_t auto_key;
    struct MemNode *head;
} *s_mem;

#define HEAD_SIZE sizeof(struct MemNode)
#define NO_USED_KEY 0
#define IDENTIFICATION_MAX 3

static struct MemNode *findKey(uint32_t key);

int initShareMemory(size_t size)
{
    unsigned char *ptr = NULL;
    if((ptr = mmap(0, size + sizeof(*s_mem), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
        return -1;
    s_mem = (void*)ptr;
    ptr += sizeof(*s_mem);
    pthread_mutex_init(&s_mem->lock, NULL);
    s_mem->size = size + sizeof(*s_mem);
    s_mem->auto_key = UINT32_MAX;

    s_mem->head = (struct MemNode*)ptr;
    s_mem->head->next = s_mem->head;
    s_mem->head->pre = s_mem->head;
    s_mem->head->info.base = ptr + HEAD_SIZE;
    s_mem->head->info.size = size - HEAD_SIZE;
    s_mem->head->info.key = NO_USED_KEY;
    return 0;
}

void destroyShareMemory(void)
{
    munmap(s_mem, s_mem->size);
}

void *shareMalloc(size_t size, uint32_t key)
{
    if(key == NO_USED_KEY)
        return NULL;
    pthread_mutex_lock(&s_mem->lock);
    struct MemNode *it_node = s_mem->head, *available_node = NULL;
    size_t new_size = size + HEAD_SIZE;
    if(key == AUTO_KEY) {
        key = s_mem->auto_key;
        while (findKey(s_mem->auto_key) != NULL)
        {
            s_mem->auto_key--;
            if(s_mem->auto_key < IDENTIFICATION_MAX)
                s_mem->auto_key = UINT32_MAX;
            if(s_mem->auto_key == key)
                return NULL;
        }
        key = s_mem->auto_key--;
    }
    do {
        if(it_node->info.key == NO_USED_KEY && it_node->info.size >= new_size) {
            if(!available_node || available_node->info.size > it_node->info.size)
                available_node = it_node;
        }
        if(key != AUTO_KEY && key != ANONYMOUS_KEY && it_node->info.key == key)
            goto ERR;
        it_node = it_node->next;
    } while(it_node != s_mem->head);
    if(!available_node)
        goto ERR;
    unsigned char *ptr = (unsigned char*)available_node->info.base + available_node->info.size - new_size;
    struct MemNode *new_node = (struct MemNode *)ptr;
    ptr += HEAD_SIZE;
    new_node->info.size = size;
    new_node->info.key = key;
    new_node->info.base = ptr;

    new_node->next = available_node->next;
    new_node->next->pre = new_node;
    available_node->next = new_node;
    new_node->pre = available_node;
    available_node->next->pre = available_node;
    available_node->info.size -= new_size;

    pthread_mutex_unlock(&s_mem->lock);
    return new_node->info.base;

    ERR:
    pthread_mutex_unlock(&s_mem->lock);
    return NULL;
}

void shareFree(void *ptr)
{
    if (!ptr)
        return;
    struct MemNode *node = (struct MemNode *)((unsigned char*)(ptr - HEAD_SIZE));
    pthread_mutex_lock(&s_mem->lock);
    struct MemNode *pre_node = node->pre;
    struct MemNode *next_node = node->next;
    node->info.key = NO_USED_KEY;

    //必须先合并后面的节点，再合并前面节点
    if (next_node->info.key == NO_USED_KEY && (char*)node->info.base + node->info.size == (void*)next_node) {
        node->info.size += next_node->info.size + HEAD_SIZE;
        next_node->next->pre = node;
        node->next = next_node->next;
    }
    if (pre_node->info.key == NO_USED_KEY && (char*)pre_node->info.base + pre_node->info.size == (void*)node) {
        pre_node->info.size += node->info.size + HEAD_SIZE;
        node->next->pre = pre_node;
        pre_node->next = node->next;
    }
    pthread_mutex_unlock(&s_mem->lock);
}

uint32_t getShareKey(void *ptr)
{
    if(!ptr)
        return NO_USED_KEY;
    struct MemNode *node = (struct MemNode *)((unsigned char*)(ptr - HEAD_SIZE));
    return node->info.key;
}

void *getShareMemory(uint32_t key)
{
    if(key < IDENTIFICATION_MAX)
        return NULL;
    pthread_mutex_lock(&s_mem->lock);
    void *ptr = findKey(key)->info.base;
    pthread_mutex_unlock(&s_mem->lock);
    return ptr;
}

static struct MemNode *findKey(uint32_t key)
{
    struct MemNode *it_node = s_mem->head;
    do {
        if(it_node->info.key == key)
            return it_node;
        it_node = it_node->next;
    } while(it_node != s_mem->head);
    return NULL;
}