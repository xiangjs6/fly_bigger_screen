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
        return -1;;
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
    if(p_start.x + size.width > image.size.width || p_start.y + size.height > image.size.height)
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