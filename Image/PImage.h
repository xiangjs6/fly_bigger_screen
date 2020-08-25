//
// Created by xjs on 2020/7/15.
//

#ifndef FLY_BIGGER_SCREEN_PIMAGE_H
#define FLY_BIGGER_SCREEN_PIMAGE_H
#include <stdint.h>
#include "../Util/Util.h"

typedef struct {
    unsigned char blue;
    unsigned char gree;
    unsigned char red;
    unsigned char alpha;
} BYTE_ALIGNED PixelBGRA;
//typedef unsigned char PixelBGRA[4];
typedef PixelBGRA *PPixelBGRA;

typedef struct {
    unsigned char blue;
    unsigned char gree;
    unsigned char red;
} BYTE_ALIGNED PixelBGR;
//typedef unsigned char PixelBGR[3];
typedef PixelBGR *PPixelBGR;

typedef PixelBGR Pixel;
typedef PPixelBGR PPixel;
typedef struct {
    int32_t width;
    int32_t height;
} Rect;

typedef struct {
    PPixel data;
    Rect size;
} PImage;

typedef struct {
    PPixel *data;
    Rect size;
} Roi;

typedef struct {
    int32_t x;
    int32_t y;
} Point;

#define PIXEL_DEPTH 256
#define PIXEL_LENGTHBGR(n) (n) * sizeof(PixelBGR)
#define PIXEL_LENGTHBGRA(n) (n) * sizeof(PixelBGRA)

#define RECT_LENGTH(size) ((size)).width * ((size)).height
#define PIXEL_LENGTH(n) (n) * sizeof(Pixel)
#define ORIGIN_POINT (Point){0, 0}

int imageCopy(PImage dst_image, PImage src_image, Point dst_start_p, Point src_start_p, Rect size);
int RoiImage(Roi *roi, PImage image, Point p_start, Rect size);
int RoiCopy(Roi *dst_roi, Roi *src_roi);
int imageResize(PImage src, PImage dst, Rect size);

void BGR2BGRA(PPixelBGR in, PPixelBGRA out, int rect_size);
void BGRA2BGR(PPixelBGRA in, PPixelBGR out, int rect_size);
#endif //FLY_BIGGER_SCREEN_PIMAGE_H
