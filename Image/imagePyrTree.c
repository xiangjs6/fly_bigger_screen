//
// Created by xjs on 2020/8/11.
//

#include "imagePyrTree.h"

static void image_copy(StackDataType *src, StackDataType *dst);
static int image_malloc(StackDataType *data);
static void image_free(StackDataType *data);
StackOpts opts = {.malloc = image_malloc, .free = image_free, .copy = image_copy};

static int generateHelper(StackHead *stack, PImage image, int layer, int odd_even);

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
    if (generateHelper(&tree->stack, image, tree->layer, 0) < 0) {
        cleanStack(&tree->stack);
        return -1;
    }
    return 0;
}

static int generateHelper(StackHead *stack, PImage image, int layer, int odd_even)
{
    if (!layer) {
        ImagePyrDataType node;
        node.image = image;
        node.odd_even = odd_even % 2;
        node.node_layer = layer;
        putStack(stack, (StackDataType){.p_val = &node});
        return 0;
    }
    int res = 0;
    Rect size = {.width = image.size.width / 2, .height = image.size.height / 2};
    if (!odd_even) {
        size.width += image.size.width % 2;
        size.height += image.size.height % 2;
    }
    PImage dst_image;
    dst_image.size = size;
    dst_image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(size)));
    if (!dst_image.data)
        return -1;
    if ((res = imageDownSize(image, dst_image, odd_even)) < 0)
        goto err;
    if ((res = generateHelper(stack, dst_image, layer - 1, odd_even)) < 0)
        goto err;
    if ((res = generateHelper(stack, dst_image, layer - 1, odd_even + 1)) < 0)
        goto err;
    err:
    free(dst_image.data);
    return res;
}

static void image_copy(StackDataType *src, StackDataType *dst)
{
    ImagePyrDataType *p_pyr_src = src->p_val;
    ImagePyrDataType *p_pyr_dst = dst->p_val;
    p_pyr_dst->node_layer = p_pyr_src->node_layer;
    p_pyr_dst->odd_even = p_pyr_src->odd_even;
    imageCopy(p_pyr_dst->image, p_pyr_src->image, ORIGIN_POINT, ORIGIN_POINT, p_pyr_src->image.size);
}

static int image_malloc(StackDataType *data)
{
    ImagePyrDataType *node = data->p_val = malloc(sizeof(ImagePyrDataType));
    if (!node)
        return -1;
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