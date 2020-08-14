//
// Created by xjs on 2020/8/3.
//

#ifndef FLY_BIGGER_SCREEN_PROTOCOLS_H
#define FLY_BIGGER_SCREEN_PROTOCOLS_H
#include <stdint.h>
#include "../Util/Util.h"
#include "../Image/PImage.h"
#include "../Image/ImageHash.h"


#define LABEL_SIZE sizeof(((header_protocol*)NULL)->label)
#define IMAGE_RESPONSE_HEAD_SIZE sizeof(((image_response_protocol*)NULL)->head)
#define IMAGE_REQUST_HEAD_SIZE sizeof(image_requst_protocol)

enum label_type {REQUST_IMAGE, RESPONSE_IMAGE};
enum image_type {NEW, OLD, INCRESS};

typedef struct {
    uint32_t seq;
} BYTE_ALIGNED image_requst_protocol;

typedef struct {
    struct {
        uint32_t seq;
        u_char type;
        Point point;
        Rect image_size;
        ImageVal index_val;
        int layer;
        uint32_t len;
    } BYTE_ALIGNED head;
    PPixelBGR data;
} BYTE_ALIGNED image_response_protocol;



typedef struct {
    u_char label;
    union {
        image_requst_protocol request_image;
        image_response_protocol response_image;
    };
} BYTE_ALIGNED header_protocol;

int putLabelToNet(enum label_type label, void *out, size_t len);
int getLabelFromNet(void *in, size_t len, enum label_type *label);
int image_response_head_to_net(image_response_protocol *in, void *out, size_t len);
int net_to_image_head_response(void *in, size_t len, image_response_protocol *out);
int image_requst_to_net(image_requst_protocol *in, void *out, size_t len);
int net_to_image_requst(void *in, size_t len, image_requst_protocol *out);
#endif //FLY_BIGGER_SCREEN_PROTOCOLS_H
