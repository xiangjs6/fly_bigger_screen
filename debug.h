//
// Created by xjs on 2020/8/3.
//

#ifndef FLY_BIGGER_SCREEN_DEBUG_H
#define FLY_BIGGER_SCREEN_DEBUG_H
#include <stdlib.h>
#include "Image/PImage.h"

int initDebug(void);
void destoryDebug(void);
void showImage(void *ptr, size_t rect_size);
void writetofile(PImage image);
void openImage(PImage *image, char *path);
#endif //FLY_BIGGER_SCREEN_DEBUG_H
