//
// Created by xjs on 2020/8/11.
//

#include "ImagePyrTree.h"
#include <memory.h>

static void image_copy(StackDataType *src, StackDataType *dst);
static int image_malloc(StackDataType *data);
static void image_free(StackDataType *data);
StackOpts opts = {.malloc = image_malloc, .free = image_free, .copy = image_copy};

static int generateHelper(StackHead *stack, PImage image, int layer);

Rect image_pyr_size;

ImagePyrTree initImagePyrTree(int layer)
{
    ImagePyrTree tree;
    tree.stack = initStack(opts);
    tree.layer = layer;
    return tree;
}

void destoryImagePyrTree(ImagePyrTree *tree)
{
    destoryStack(&tree->stack);
}

int imagePyramid(ImagePyrTree *tree, PImage image)
{
    if (generateHelper(&tree->stack, image, tree->layer) < 0 ) {
        cleanStack(&tree->stack);
        return -1;
    }
    return 0;
}

int putPyramid(ImagePyrTree *tree, ImagePyrDataType pyramid)
{
    if (tree->stack.size < 4) {
        return putStack(&tree->stack, (StackDataType){.p_val = &pyramid});
    } else {
        while (tree->stack.size >= 4)
        {
            ImagePyrDataType all_pyramid[4];
            for (int i = 3; i >= 0; i--) {
                StackDataType data = {.p_val = &all_pyramid[i]};
                popStack(&tree->stack, &data);
            }
        }
    }
}

static int generateHelper(StackHead *stack, PImage image, int layer)
{
    if (!layer) {
        ImagePyrDataType data;
        data.image = image;
        data.node_layer = layer;
        if (putStack(stack, (StackDataType) {.p_val = &data}) < 0)
            return -1;
        return 0;
    }

    ImagePyramid pyramid;
    if (splitPyramid(image, &pyramid) < 0)
        return -1;
    for (int i = 0; i < 4; i++) {
        if (generateHelper(stack, pyramid.image[i], layer - 1) < 0)
            return -1;
    }
    return 0;
}

int mergePyramid(ImagePyramid pyramid, PImage *out)
{
}

int splitPyramid(PImage image, ImagePyramid *out)
{
    Rect size;
    //x偶 y偶
    size.width = image.size.width / 2 + image.size.width % 2;
    size.height = image.size.height / 2 + image.size.height % 2;
    out->image[0].data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
    if (!out->image[0].data)
        return -1;
    out->image[0].size = size;
    imageDownSize(image, out->image[0], 0, 0);

    //x偶 y奇
    size.width = image.size.width / 2 + image.size.width % 2;
    size.height = image.size.height / 2;
    out->image[1].data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
    if (!out->image[1].data)
        return -1;
    out->image[1].size = size;
    imageDownSize(image, out->image[1], 0, 1);

    //x奇 y偶
    size.width = image.size.width / 2;
    size.height = image.size.height / 2 + image.size.height % 2;
    out->image[2].data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
    if (!out->image[2].data)
        return -1;
    out->image[2].size = size;
    imageDownSize(image, out->image[2], 1, 0);

    //x奇 y奇
    size.width = image.size.width / 2;
    size.height = image.size.height / 2;
    out->image[3].data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
    if (!out->image[3].data)
        return -1;
    out->image[3].size = size;
    imageDownSize(image, out->image[3], 1, 1);

    return 0;
}

static void image_copy(StackDataType *src, StackDataType *dst)
{
    ImagePyrDataType *p_pyr_src = src->p_val;
    ImagePyrDataType *p_pyr_dst = dst->p_val;
    p_pyr_dst->node_layer = p_pyr_src->node_layer;
    imageCopy(p_pyr_dst->image, p_pyr_src->image, ORIGIN_POINT, ORIGIN_POINT, p_pyr_src->image.size);
}

static int image_malloc(StackDataType *data)
{
    ImagePyrDataType *node = data->p_val = malloc(sizeof(ImagePyrDataType));
    if (!node)
        return -1;
    memset(node, 0, sizeof(ImagePyrDataType));
    node->image.size = image_pyr_size;
    node->image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(image_pyr_size)));
    if (!node->image.data) {
        free(node);
        return -1;
    }
    return 0;
}

static void image_free(StackDataType *data)
{
    ImagePyrDataType *node = data->p_val;
    free(node->image.data);
    free(node);
}