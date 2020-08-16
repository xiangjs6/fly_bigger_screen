//
// Created by xjs on 2020/7/28.
//

#include "ShareMemory.h"
#include <pthread.h>
#include <sys/mman.h>
#include <semaphore.h>

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

static struct ShareHead{
    size_t size;
    pthread_mutex_t lock;
    //sem_t lock;
    uint32_t auto_key;
    struct MemNode *head;
} *s_mem;

#define NODE_SIZE sizeof(struct MemNode)
#define NO_USED_KEY 0
#define IDENTIFICATION_MAX 3

static struct MemNode *findKey(uint32_t key);

int initShareMemory(size_t size)
{
    unsigned char *ptr = NULL;
    if((ptr = mmap(0, size + sizeof(struct ShareHead), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
        return -1;
    s_mem = (void*)ptr;
    ptr += sizeof(struct ShareHead);
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&s_mem->lock, &attr);
    /*if (sem_init(&s_mem->lock,1,1) != 0)
        return -1;*/

    pthread_mutexattr_destroy(&attr);
    s_mem->size = size + sizeof(struct ShareHead);
    s_mem->auto_key = UINT32_MAX;

    s_mem->head = (struct MemNode*)ptr;
    s_mem->head->next = s_mem->head;
    s_mem->head->pre = s_mem->head;
    s_mem->head->info.base = ptr + NODE_SIZE;
    s_mem->head->info.size = size - NODE_SIZE;
    s_mem->head->info.key = NO_USED_KEY;
    return 0;
}

void destroyShareMemory(void)
{
    pthread_mutex_destroy(&s_mem->lock);
    //sem_destroy(&s_mem->lock);
    munmap(s_mem, s_mem->size);
}

void *shareMalloc(size_t size, uint32_t key)
{
    if(key == NO_USED_KEY)
        return NULL;
    pthread_mutex_lock(&s_mem->lock);
    //sem_wait(&s_mem->lock);
    struct MemNode *it_node = s_mem->head, *available_node = NULL;
    size_t new_size = size + NODE_SIZE;
    if(key == AUTO_KEY) {
        key = s_mem->auto_key;
        while (findKey(s_mem->auto_key) != NULL)
        {
            s_mem->auto_key--;
            if(s_mem->auto_key < IDENTIFICATION_MAX)
                s_mem->auto_key = UINT32_MAX;
            if(s_mem->auto_key == key)
                goto ERR;
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
    ptr += NODE_SIZE;
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
    //sem_post(&s_mem->lock);
    return new_node->info.base;

    ERR:
    //sem_post(&s_mem->lock);
    pthread_mutex_unlock(&s_mem->lock);
    return NULL;
}

void shareFree(void *ptr)
{
    if (!ptr)
        return;
    struct MemNode *node = (struct MemNode *)((unsigned char*)(ptr - NODE_SIZE));
    pthread_mutex_lock(&s_mem->lock);
    //sem_wait(&s_mem->lock);
    struct MemNode *pre_node = node->pre;
    struct MemNode *next_node = node->next;
    node->info.key = NO_USED_KEY;

    //必须先合并后面的节点，再合并前面节点
    if (next_node->info.key == NO_USED_KEY && (char*)node->info.base + node->info.size == (void*)next_node) {
        node->info.size += next_node->info.size + NODE_SIZE;
        next_node->next->pre = node;
        node->next = next_node->next;
    }
    if (pre_node->info.key == NO_USED_KEY && (char*)pre_node->info.base + pre_node->info.size == (void*)node) {
        pre_node->info.size += node->info.size + NODE_SIZE;
        node->next->pre = pre_node;
        pre_node->next = node->next;
    }
    pthread_mutex_unlock(&s_mem->lock);
    //sem_post(&s_mem->lock);
}

uint32_t getShareKey(void *ptr)
{
    if(!ptr)
        return NO_USED_KEY;
    struct MemNode *node = (struct MemNode *)((unsigned char*)(ptr - NODE_SIZE));
    return node->info.key;
}

void *getShareMemory(uint32_t key)
{
    if(key < IDENTIFICATION_MAX)
        return NULL;
    //sem_wait(&s_mem->lock);
    pthread_mutex_lock(&s_mem->lock);
    void *ptr = findKey(key)->info.base;
    pthread_mutex_unlock(&s_mem->lock);
    //sem_post(&s_mem->lock);
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