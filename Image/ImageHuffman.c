//
// Created by xjs on 2020/8/25.
//

#include "ImageHuffman.h"
#include "../Util/Util.h"

static int put_code(char **ptr, char *str, int pos);
static int get_code(char **ptr, char *c, int pos);

int ImageHuffmanEncode(PImage image, HuffmanTree *tree, void *out, size_t *len)
{
    HuffmanCode code[PIXEL_DEPTH];
    int count[PIXEL_DEPTH] = {};
    for (int i = 0; i < image.size.height; i++) {
        for (int j = 0; j < image.size.width; j++) {
            PPixel pixel = &image.data[i * image.size.width + j];
            count[pixel->blue]++;
            count[pixel->gree]++;
            count[pixel->red]++;
        }
    }
    int data_index = 0;
    HuffmanData data[PIXEL_DEPTH];
    for (int i = 0; i < PIXEL_DEPTH; i++) {
        if (!count[i])
            continue;
        data[data_index].weight = count[i];
        data[data_index].value = i;
        data_index++;
    }
    tree->size = HUFFMAN_NODE_SIZE(data_index);
    tree->tree = malloc(sizeof(HuffmanNode) * tree->size);
    tree->leaf_size = data_index;
    if (!tree->tree)
        return -1;
    generateHuffmanTree(data, data_index, tree, 0);
    HuffmanNode t = tree->tree[tree->size - 1];
    HuffmanTreeToCode(tree, code, 0, NULL);

    char *ptr = out;
    int pos = 0;
    for (int i = 0; i < image.size.height; i++) {
        for (int j = 0; j < image.size.width; j++) {
            unsigned char *p_pixel = (unsigned char*)&image.data[i * image.size.width + j];
            for (int k = 0; k < sizeof(Pixel); k++) {
                char *p_c = code[*p_pixel].code;
                pos = put_code(&ptr, p_c, pos);
                p_pixel++;
            }
        }
    }
    *len = ptr - (char*)out + 1;
    return pos;
}

void ImageHuffmanDecode(void *code, HuffmanTree *tree, PImage *out)
{
    int pos = 0;
    char *ptr = code;
    char c;
    for (int i = 0; i < out->size.height; i++) {
        for (int j = 0; j < out->size.width; j++) {
            unsigned char *p_pixel = (unsigned char*)&out->data[i * out->size.width + j];
            for (int k = 0; k < sizeof(Pixel); k++) {
                int parent_node = tree->size - 1;
                int left = tree->tree[parent_node].children[0];
                int right = tree->tree[parent_node].children[1];
                while (left != -1 && right != -1)
                {
                    pos = get_code(&ptr, &c, pos);
                    parent_node = tree->tree[parent_node].children[c];
                    left = tree->tree[parent_node].children[0];
                    right = tree->tree[parent_node].children[1];
                }
                *p_pixel = (unsigned char)tree->tree[parent_node].data.value;
                p_pixel++;
            }
        }
    }
}

static int put_code(char **ptr, char *str, int pos)
{
    while (*str)
    {
        SET_BIT(**ptr, *str - '0', pos);
        pos = (pos + 1) % 8;
        if (!pos)
            (*ptr)++;
        str++;
    }
    return pos;
}

static int get_code(char **ptr, char *c, int pos)
{
    *c = GET_BIT(**ptr, pos);
    pos = (pos + 1) % 8;
    if (!pos)
        (*ptr)++;
    return pos;
}