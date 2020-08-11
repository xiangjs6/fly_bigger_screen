//
// Created by xjs on 2020/7/19.
//

#ifndef FLY_BIGGER_SCREEN_IMAGEHASH_H
#define FLY_BIGGER_SCREEN_IMAGEHASH_H
#include <stdlib.h>
#include "../Util/Util.h"
#include "PImage.h"
#include "ImageMesh.h"

typedef struct ImageKey ImageKey;
typedef struct ImageVal ImageVal;
typedef HashMap ImageHashMap;

struct ImageKey {
    PPixel key;
    size_t len;
    //uint32_t *check_sum;
    uint32_t check_sum;
};

struct ImageVal {
    int32_t index;
    Point h_mesh_point;
};

ImageHashMap craetImageHashMap(size_t size);
void destoryImageHashMap(ImageHashMap *map);
int putImageHashMap(ImageHashMap *map, ImageKey key, ImageVal val, ImageVal *old_val);
int getImageHashMap(ImageHashMap *map, ImageKey key, ImageVal *val);
int delImageHashMap(ImageHashMap *map, ImageKey key);

static int image_cmp(HashKey key1, HashKey key2);
static void image_copy(HashDataType *src, HashDataType *dst);
static uint32_t image_hash_code(HashMap *map, HashKey key);
static int image_malloc(HashDataType *data);
static void image_free(HashDataType *data);
#endif //FLY_BIGGER_SCREEN_IMAGEHASH_H
