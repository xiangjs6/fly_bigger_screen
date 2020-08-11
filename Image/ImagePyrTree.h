//
// Created by xjs on 2020/8/11.
//

#ifndef FLY_BIGGER_SCREEN_IMAGEPYRTREE_H
#define FLY_BIGGER_SCREEN_IMAGEPYRTREE_H

#include "PImage.h"

typedef struct ImagePyrDataType {
    PImage image;
    int node_layer;
    int odd_even;
} ImagePyrDataType;

typedef struct ImagePyrTree {
    StackHead stack;
    int layer;
} ImagePyrTree;

ImagePyrTree initImagePyrTree(int layer);
void destoryImagePyrTree(ImagePyrTree *tree);
int imagePyramid(ImagePyrTree *tree, PImage image);

#endif //FLY_BIGGER_SCREEN_IMAGEPYRTREE_H
