//
// Created by xjs on 2020/8/11.
//

#include "ImagePyrTree.h"
#include <memory.h>

static void image_copy(StackDataType *src, StackDataType *dst);
static int image_malloc(StackDataType *data);
static void image_free(StackDataType *data);
StackOpts opts = {.malloc = image_malloc, .free = image_free, .copy = image_copy};

static int generateHelper(StackHead *stack, PImage image, int layer, int odd_even_num);

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
    if (generateHelper(&tree->stack, image, tree->layer, 0) < 0 ) {
        cleanStack(&tree->stack);
        return -1;
    }
    return 0;
}

int putPyramid(ImagePyrTree *tree, ImagePyrDataType pyramid)
{
    int cur_index = pyramid.node_layer;
    int i = 4, res = 0;
    StackDataType data;
    ImagePyramid image_pyramid;
    ImagePyrDataType all_pyramid[4];
    ImagePyrDataType new_image_pyr;

    image_pyr_size = pyramid.image.size;
    if (putStack(&tree->stack, (StackDataType){.p_val = &pyramid}) < 0)
        return -1;
    while (tree->stack.size >= 4)
    {
        for (i = 0; i < 4; i++) {
            ImagePyrDataType *p_pyr = getStack(&tree->stack, STACK_TOP)->p_val;
            all_pyramid[i].image.size = p_pyr->image.size;
            all_pyramid[i].image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(p_pyr->image.size)));
            if (!all_pyramid[i].image.data) {
                i--;
                res = -1;
                goto ret;
            }
            data.p_val = &all_pyramid[i];
            popStack(&tree->stack, &data);
            if (all_pyramid[i].node_layer != cur_index) {
                res = 2;
                goto ret;
            }
            image_pyramid.image[i] = all_pyramid[i].image;
        }
        mergePyramid(image_pyramid, &new_image_pyr.image);
        new_image_pyr.node_layer = ++cur_index;
        data.p_val = &new_image_pyr;

        image_pyr_size = new_image_pyr.image.size;
        int _res = putStack(&tree->stack, data);
        free(new_image_pyr.image.data);
        if (_res < 0) {
            res = -2;
            goto ret;
        }
        res = 1;
    }
ret:
    switch (res)
    {
        case 1:
            for (int k = 0; k < 4; k++)
                free(all_pyramid[k].image.data);
            break;
        case -1:
        case 2:
            for (int j = i; j >= 0; j--) {
                data.p_val = &all_pyramid[j];
                image_pyr_size = all_pyramid[j].image.size;
                putStack(&tree->stack, data);
                free(all_pyramid[j].image.data);
            }
            break;
        case -2:
            popStack(&tree->stack, NULL);
            break;
    }

    return res;
}

static int generateHelper(StackHead *stack, PImage image, int layer, int odd_even_num)
{
    if (!layer) {
        ImagePyrDataType data;
        data.image = image;
        data.node_layer = layer;
        data.odd_even_num = odd_even_num;
        image_pyr_size = image.size;
        if (putStack(stack, (StackDataType) {.p_val = &data}) < 0)
            return -1;
        return 0;
    }

    ImagePyramid pyramid;
    if (splitPyramid(image, &pyramid) < 0)
        return -1;
    for (int i = 0; i < 4; i++) {
        if (generateHelper(stack, pyramid.image[i], layer - 1, i) < 0)
            return -1;
    }
    return 0;
}

int mergePyramid(ImagePyramid pyramid, PImage *out)
{
    static const Point odd_even[4] = {{.x = 0, .y = 0}, {.x = 0, .y = 1}, {.x = 1, .y = 0}, {.x = 1, .y = 1}};

    Rect size = {0, 0};
    size.width = pyramid.image[0].size.width + pyramid.image[3].size.width;
    size.height = pyramid.image[0].size.height + pyramid.image[3].size.height;

    out->size = size;
    out->data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
    if (!out->data)
        return -1;
    for (int k = 0; k < 4; k++) {
        PImage *p_src = &pyramid.image[k];
        for (int i = 0; i < p_src->size.height; i++) {
            for (int j = 0; j < p_src->size.width; j++) {
                int row = i * 2 + odd_even[k].y;
                int col = j * 2 + odd_even[k].x;
                out->data[row * size.width + col] = p_src->data[i * p_src->size.width + j];
            }
        }
    }
    return 0;
}

int splitPyramid(PImage image, ImagePyramid *out)
{
    static const Point odd_even[4] = {{.x = 0, .y = 0}, {.x = 0, .y = 1}, {.x = 1, .y = 0}, {.x = 1, .y = 1}};
    Rect size;
    for (int i = 0; i < 4; i++) {
        size.width = image.size.width / 2 + ((odd_even[i].x + 1) % 2) * (image.size.width % 2);
        size.height = image.size.height / 2 + ((odd_even[i].y + 1) % 2) * (image.size.height % 2);
        out->image[i].data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
        if (!out->image[i].data)
            goto err;
        out->image[i].size = size;
        imageDownSize(image, out->image[i], odd_even[i].x, odd_even[i].y);
    }
    return 0;
err:
    for (int i = 0; i < 4; i++)
        free(out->image[i].data);
    return -1;
}

static void image_copy(StackDataType *src, StackDataType *dst)
{
    ImagePyrDataType *p_pyr_src = src->p_val;
    ImagePyrDataType *p_pyr_dst = dst->p_val;
    p_pyr_dst->node_layer = p_pyr_src->node_layer;
    p_pyr_dst->odd_even_num = p_pyr_src->odd_even_num;
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
