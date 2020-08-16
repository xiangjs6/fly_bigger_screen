//
// Created by xjs on 2020/8/2.
//

#ifndef FLY_BIGGER_SCREEN_PROGRAM_PROTOCOL_H
#define FLY_BIGGER_SCREEN_PROGRAM_PROTOCOL_H

#include "../Image/PImage.h"

struct encode_response_protocol {
    uint32_t seq;
    uint32_t mesh_mark_key;
    uint32_t pyramids_key;
    //uint32_t mesh_head_array_key;
    Rect mesh_num_size;
    //Rect mesh_size;
    int curent_array_index;
};

struct decode_requst_protocol {
    uint32_t seq;
    uint32_t mesh_mark_key;
    uint32_t pyramids_key;
    //uint32_t mesh_head_key;
    Rect mesh_num_size;
    //Rect mesh_size;
};

struct encode_requst_protocol {
    uint32_t seq;
};

struct decode_response_protocol {
    uint32_t seq;
    uint32_t mesh_mark_key;
    uint32_t pyramids_key;
};

struct program_protocol {
    enum {
        REQUST_ENCODE_IMAGE, REQUST_DECODE_IMAGE, RESPONSE_ENCODE_IMAGE, RESPONSE_DECODE_IMAGE
    } protocol_label;
    union {
        struct encode_response_protocol response_encode;
        struct decode_requst_protocol requst_decode;
        struct encode_requst_protocol requst_encode;
        struct decode_response_protocol response_decode;
    };
};
#endif //FLY_BIGGER_SCREEN_PROGRAM_PROTOCOL_H
