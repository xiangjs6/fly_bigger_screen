//
// Created by xjs on 2020/7/17.
//

#ifndef FLY_BIGGER_SCREEN_HASHMAP_H
#define FLY_BIGGER_SCREEN_HASHMAP_H

#include <stdint.h>
#include <stdlib.h>

//通用HashMap容器部分 32位hashcode
//无法多线程
typedef struct HashMap HashMap;
typedef struct HashDataType HashDataType;
typedef struct HashDataOpts HashDataOpts;
typedef struct HashNode HashNode;
typedef union {void* p_key; int64_t key;} HashKey;
typedef union {void* p_val; int64_t val;} HashVal;

struct HashDataType {
    HashKey key;
    HashVal val;
};

struct HashDataOpts {
    void (*copy)(HashDataType *src, HashDataType *dst);
    int (*cmp)(HashKey key1, HashKey key2);
    uint32_t (*hash_code)(HashMap *map, HashKey key);
    int (*malloc)(HashDataType *data);
    void (*free)(HashDataType *data);
};

struct HashNode {
    HashDataType data;
    HashNode *next;  //key冲突时，通过next指针进行连接
};

struct HashMap {
    size_t size;
    HashDataOpts opts;
    HashNode **table;
};

HashMap creatHashMap(size_t size, HashDataOpts opts);
void destoryHashMap(HashMap *map);
int putHashMapbyCode(HashMap *map, HashDataType key_val, uint32_t hash_code);
int putHashMap(HashMap *map, HashDataType key_val);
int getHashMap(HashMap *map, HashDataType *key_val);
int delHashNodebyKey(HashMap *map, HashKey key);
int delHashNodebyCode(HashMap *map, HashKey key, uint32_t hash_code);
#endif //FLY_BIGGER_SCREEN_HASHMAP_H
