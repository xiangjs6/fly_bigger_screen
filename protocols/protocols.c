//
// Created by xjs on 2020/8/3.
//

#include <netinet/in.h>
#include <memory.h>
#include "protocols.h"

int putLabelToNet(enum label_type label, void *out, size_t len)
{
    if(len < LABEL_SIZE)
        return -1;
    *(u_char*)out = label;
    return LABEL_SIZE;
}

int getLabelFromNet(void *in, size_t len, enum label_type *label)
{
    if(len < LABEL_SIZE)
        return -1;
    *label = *(u_char*)in;
    return 0;
}

int image_response_head_to_net(image_response_protocol *in, void *out, size_t len)
{
    if (IMAGE_RESPONSE_HEAD_SIZE + in->head.len > len)
        return -1;
    u_char *ptr = out;
    //序号
    *(uint32_t*)ptr = htonl(in->head.seq);
    ptr += sizeof(uint32_t);
    //类型
    *ptr = in->head.type;
    ptr++;
    //位置
    *(int32_t*)ptr = htonl(in->head.point.x);
    ptr += sizeof(int32_t);
    *(int32_t*)ptr = htonl(in->head.point.y);
    ptr += sizeof(int32_t);
    //图片大小
    *(int32_t*)ptr = htonl(in->head.image_size.width);
    ptr += sizeof(int32_t);
    *(int32_t*)ptr = htonl(in->head.image_size.height);
    ptr += sizeof(int32_t);
    //数据索引
    *(int32_t*)ptr = htonl(in->head.index_val.index);
    ptr += sizeof(int32_t);
    *(int32_t*)ptr = htonl(in->head.index_val.h_mesh_point.x);
    ptr += sizeof(int32_t);
    *(int32_t*)ptr = htonl(in->head.index_val.h_mesh_point.y);
    ptr += sizeof(int32_t);
    //数据长度
    *(uint32_t*)ptr = htonl(in->head.len);
    ptr += sizeof(uint32_t);
    //数据
    /*if (in->head.type == NEW)
        memcpy(ptr, in->data, in->head.len);
    ptr += in->head.len;*/
    return ptr - (u_char*)out;
}

int net_to_image_head_response(void *in, size_t len, image_response_protocol *out)
{
    if(len < IMAGE_RESPONSE_HEAD_SIZE)
        return -1;
    u_char *ptr = in;
    //序号
    out->head.seq = ntohl(*(uint32_t*)ptr);
    ptr += sizeof(uint32_t);
    //类型
    out->head.type = *ptr;
    ptr++;
    //位置
    out->head.point.x = ntohl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    out->head.point.y = ntohl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    //图片大小
    out->head.image_size.width = ntohl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    out->head.image_size.height = ntohl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    //数据索引
    out->head.index_val.index = htonl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    out->head.index_val.h_mesh_point.x = htonl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    out->head.index_val.h_mesh_point.y = htonl(*(int32_t*)ptr);
    ptr += sizeof(int32_t);
    //数据长度
    out->head.len = htonl(*(uint32_t*)ptr);
    ptr += sizeof(uint32_t);
    //数据
    /*if (len - ((void*)ptr - in) != out->head.len)
        return -1;
    if (out->head.type == NEW)
        memcpy(out->data, ptr, out->head.len);*/
    return 0;
}

int image_requst_to_net(image_requst_protocol *in, void *out, size_t len)
{
    if(len < IMAGE_REQUST_HEAD_SIZE)
        return -1;
    u_char *ptr = out;
    //序号
    *(uint32_t*)ptr = in->seq;
    ptr += sizeof(uint32_t);
    return ptr - (u_char*)out;
}

int net_to_image_requst(void *in, size_t len, image_requst_protocol *out)
{
    if(len < IMAGE_REQUST_HEAD_SIZE)
        return -1;
    u_char *ptr = in;
    //序号
    out->seq = *(uint32_t*)ptr;
    ptr += sizeof(uint32_t);
    return 0;
}