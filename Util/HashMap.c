//
// Created by xjs on 2020/7/17.
//

#include "HashMap.h"
#include <stdio.h>
#include <memory.h>

static uint32_t _def_hash_code(HashMap *map, HashKey key);
static int _def_cmp(HashKey key1, HashKey key2);
static void _def_copy(HashDataType *src, HashDataType *dst);
static int _def_malloc(HashDataType *data);
static void _def_free(HashDataType *data);

static HashDataOpts defalut_opts = {.copy = _def_copy, .cmp = _def_cmp, .hash_code = _def_hash_code, .malloc = _def_malloc, .free = _def_free};
typedef void(*PFunc)(void);

HashMap creatHashMap(size_t size, HashDataOpts opts)
{
    HashMap map = {size, opts};
    map.table = (HashNode **)malloc(sizeof(HashNode*) * size);
    if(!map.table) {
        map.size = 0;
        return map;
    }
    memset(map.table, 0, sizeof(HashNode*) * size);
    PFunc *p_end = (PFunc*)&map.opts + sizeof(HashDataOpts) / sizeof(PFunc);
    for(PFunc *p_opts = (PFunc*)&map.opts, *p_def_opts = (PFunc*)&defalut_opts; p_opts < p_end; p_opts++, p_def_opts++) {
        if (!*p_opts)
            *p_opts = *p_def_opts;
    }
    return map;
}

void destoryHashMap(HashMap *map)
{
    for(int i = 0; i < map->size; i++) {
        while(map->table[i])
        {
            HashNode *node = map->table[i];
            map->table[i] = node->next;
            map->opts.free(&node->data);
            free(node);
        }
    }
    free(map->table);
}

int putHashMapbyCode(HashMap *map, HashDataType key_val, uint32_t hash_code)
{
    if(hash_code >= map->size)
        return -1;
    HashNode *node = map->table[hash_code];
    while(node && map->opts.cmp(key_val.key, node->data.key))//寻找是否有key值一样,如果Hash冲撞太多，得改进hash算法和链表换二叉树
        node = node->next;
    if(node) {
        map->opts.copy(&key_val, &node->data);
        return 1;
    }
    node = malloc(sizeof(HashNode));
    if(!node)
        return -1;
    if(map->opts.malloc(&node->data) < 0) {
        free(node);
        return -1;
    }
    map->opts.copy(&key_val, &node->data);
    node->next = map->table[hash_code];
    map->table[hash_code] = node;
    return 0;
}

int putHashMap(HashMap *map, HashDataType key_val)
{
    return putHashMapbyCode(map, key_val, map->opts.hash_code(map, key_val.key));
}

int getHashMap(HashMap *map, HashDataType *key_val)
{
    if (!map || !key_val)
        return -1;
    uint32_t hash_code = map->opts.hash_code(map, key_val->key);
    HashNode *node = map->table[hash_code];
    while(node && map->opts.cmp(key_val->key, node->data.key))
        node = node->next;
    if(node) {
        map->opts.copy(&node->data, key_val);
        return 1;
    }
    return 0;
}

int delHashNodebyKey(HashMap *map, HashKey key)
{
    return delHashNodebyCode(map, key, map->opts.hash_code(map, key));
}

int delHashNodebyCode(HashMap *map, HashKey key, uint32_t hash_code)
{
    if(hash_code >= map->size)
        return -1;
    HashNode *node = map->table[hash_code], *pre_node = NULL;
    while(node && map->opts.cmp(key, node->data.key)) {
        pre_node = node;
        node = node->next;
    }
    if(!node)
        return -1;
    if(pre_node)
        pre_node->next = node->next;
    else
        map->table[hash_code] = node->next;
    map->opts.free(&node->data);
    free(node);
    return 0;
}

int addressHashMap(HashMap *map, HashKey key, HashNode **out)
{
    uint32_t hash_code = map->opts.hash_code(map, key);
    if(hash_code >= map->size)
        return -1;
    HashNode *node = map->table[hash_code];
    while(node && map->opts.cmp(key, node->data.key))//寻找是否有key值一样,如果Hash冲撞太多，得改进hash算法和链表换二叉树
        node = node->next;
    if(node) {
        *out = node;
        return 1;
    } else {
        node = malloc(sizeof(HashNode));
        if(!node)
            return -1;
        if(map->opts.malloc(&node->data) < 0) {
            free(node);
            return -1;
        }
        node->next = map->table[hash_code];
        map->table[hash_code] = node;
        *out = node;
        return 0;
    }
}

static uint32_t _def_hash_code(HashMap *map, HashKey key)
{
    return key.key % map->size;
}

static int _def_cmp(HashKey key1, HashKey key2)
{
    return key1.key - key2.key;
}

static void _def_copy(HashDataType *src, HashDataType *dst) {*dst = *src;}

static int _def_malloc(HashDataType *data)
{
    data->val.val = 0;
    return 0;
}

static void _def_free(HashDataType *data) {}