//
// Created by xjs on 2020/7/19.
//

#include "ImageHash.h"
#include "../Util/Util.h"
#include <memory.h>

static HashDataOpts image_opts = {
        .cmp = image_cmp,
        .hash_code = image_hash_code,
        .copy = image_copy,
        .malloc = image_malloc,
        .free = image_free
};

ImageHashMap craetImageHashMap(size_t size)
{
    HashMap map = creatHashMap(size, image_opts);
    return map;
}

void destoryImageHashMap(ImageHashMap *map)
{
    destoryHashMap(map);
}

int getImageHashMap(ImageHashMap *map, ImageKey key, ImageVal *val)
{
    HashDataType key_val = {&key, val};
    return getHashMap(map, &key_val);
}

int putImageHashMap(ImageHashMap *map, ImageKey key, ImageVal val, ImageVal *old_val)
{
    //uint16_t check_sum = checksum((uint16_t*)key.key, key.len);
    //*key.check_sum = check_sum;
    if(old_val) {
        ImageKey _key = key;
        HashDataType _key_val = {.key.p_key = &_key, .val.p_val = old_val};
        getHashMap(map, &_key_val);
    }
    HashDataType key_val = {.key.p_key = &key, .val.p_val = &val};
    return putHashMap(map, key_val);
}

int delImageHashMap(ImageHashMap *map, ImageKey key)
{
    HashKey _key = {&key};
    return delHashNodebyKey(map, _key);
}

static int image_cmp(HashKey key1, HashKey key2)
{
    uint32_t res;
    ImageKey *p_key1 = key1.p_key, *p_key2 = key2.p_key;
    if(p_key1->len != p_key2->len)
        return -1;
    /*if((res = *p_key1->check_sum - *p_key2->check_sum))
        return res;*/
    if((res = p_key1->check_sum - p_key2->check_sum))
        return res;
    if(p_key1->key == p_key2->key)
        return 0;
    return memcmp(p_key1->key, p_key2->key, p_key1->len);
}

static uint32_t image_hash_code(HashMap *map, HashKey key)
{
    //ImageKey *p_key = key.p_key;
    //return *p_key->check_sum % map->size;
    ImageKey *p_key = key.p_key;
    p_key->check_sum = checksum((uint16_t*)p_key->key, p_key->len);
    return p_key->check_sum % map->size;
}

static void image_copy(HashDataType *src, HashDataType *dst)
{
    ImageVal *p_image_val = dst->val.p_val;
    ImageKey *p_image_key = dst->key.p_key;
    *p_image_key = *(ImageKey*)src->key.p_key;
    *p_image_val = *(ImageVal*)src->val.p_val;
}

static int image_malloc(HashDataType *data)
{
    char *p = malloc(sizeof(ImageKey) + sizeof(ImageVal));
    if(!p)
        return -1;
    data->key.p_key = p;
    data->val.p_val = p + sizeof(ImageKey);
    return 0;
}

static void image_free(HashDataType *data)
{
    free(data->key.p_key);//只需要释放key，val内存地址和key一起申请的
}