//
// Created by xjs on 2020/7/15.
//

#include "PImage.h"
#include <memory.h>

int imageCopy(PImage dst_image, PImage src_image, Point dst_start_p, Point src_start_p, Rect size)
{
    PPixel src_image_roi_data[size.height];
    PPixel dst_image_roi_data[size.height];
    Roi src_roi = {.size = size, .data = src_image_roi_data};
    Roi dst_roi = {.size = size, .data = dst_image_roi_data};
    if (RoiImage(&src_roi, src_image, src_start_p, size) < 0 ||
        RoiImage(&dst_roi, dst_image, dst_start_p, size) < 0)
        return -1;
    return RoiCopy(&dst_roi, &src_roi);
/*    dst_image.size = size;
    int pixel_len = PIXEL_LENGTH(size.width);
    PPixel p_dst_data = dst_image.data;
    PPixel p_src_data = src_image.data + start_p.y * src_image.size.width + start_p.x;
    for(int i = 0; i < size.height; i++){
        memcpy(p_dst_data, p_src_data, pixel_len);
        p_dst_data += size.width;
        p_src_data += src_image.size.width;
    }*/
}

int RoiImage(Roi *roi, PImage image, Point p_start, Rect size)
{
    if(p_start.x < 0 || p_start.y < 0)
        return -1;
    if(p_start.x + size.width >= image.size.width || p_start.y + size.height >= image.size.height)
        return -1;
    roi->size = size;
    PPixel p_image_data = image.data + image.size.width * p_start.y + p_start.x;
    for(int i = 0; i < size.height; i++) {
        roi->data[i] = p_image_data;
        p_image_data += image.size.width;
    }
    return 0;
}

int RoiCopy(Roi *dst_roi, Roi *src_roi)
{
    if (dst_roi->size.height < src_roi->size.height ||
        dst_roi->size.width < src_roi->size.width)
        return -1;
    for (int i = 0; i < src_roi->size.height; i++)
        memcpy(dst_roi->data[i], src_roi->data[i], PIXEL_LENGTH(src_roi->size.width));
    return 0;
}

int imageDownSize(PImage src, PImage dst, int odd_even)
{
    odd_even %= 2;
    Rect size = {.width = src.size.width / 2, .height = src.size.height / 2};
    if (!odd_even) {
        size.width += src.size.width % 2;
        size.height += src.size.height % 2;
    }
    if (size.width != dst.size.width || size.height != dst.size.height)
        return -1;
    PPixel p_src = src.data + odd_even * src.size.width + odd_even;
    PPixel p_dst = dst.data;
    for (int i = 0; i < size.height; i++) {
        for (int j = 0; j < size.width; j++) {
            *p_dst = *p_src;
            p_src += 2;
            p_dst++;
        }
        p_src += src.size.width;
    }
    return 0;
}

int imageResize(PImage src, PImage dst, Rect size)
{
    if (size.width != dst.size.width || size.height != dst.size.height)
        return -1;
    memset(dst.data, 0, PIXEL_LENGTH(RECT_LENGTH(size)));
    float scala_width = (float)src.size.width / (float)size.width;
    float scala_height = (float)src.size.height / (float)size.height;
    PPixel p_dst = dst.data;
    for (int i = 0; i < size.height; i++) {
        for (int j = 0; j < size.width; j++) {
            Point src_p;
            src_p.x = scala_width * j;
            src_p.y = scala_height * i;
            *p_dst = src.data[src_p.x + src_p.y * src.size.width];
            p_dst++;
        }
    }
    return 0;
}

void BGR2BGRA(PPixelBGR in, PPixelBGRA out, int rect_size)
{
    for (int i = 0; i < rect_size; i++) {
        out->alpha = 0;
        memcpy(out++, in++, sizeof(PixelBGR));
    }
}

void BGRA2BGR(PPixelBGRA in, PPixelBGR out, int rect_size)
{
    for (int i = 0; i < rect_size; i++) {
        memcpy(out++, in++, sizeof(PixelBGR));
    }
}
