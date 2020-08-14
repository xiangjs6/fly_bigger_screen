//
// Created by xjs on 2020/7/27.
//

#ifndef FLY_BIGGER_SCREEN_IMAGECODE_H
#define FLY_BIGGER_SCREEN_IMAGECODE_H
#include <stdbool.h>
#include "ImageMesh.h"
#include "ImagePyrTree.h"
#include "ImageHash.h"

struct pyramid_code {
    ImagePyrTree tree;
    struct pyramid_code *next;
    struct pyramid_code *pre;
};

struct pyramid_code *creat_pyramid_node(struct pyramid_code **head);
void del_pyramid_node(struct pyramid_code **head, struct pyramid_code *node);

struct code_array_type {
    bool is_used;
    MeshHead *h_mesh;
    bool *mesh_updata_mark;
    struct pyramid_code **pyramid_trees;
};
int init_code_array_type(struct code_array_type *p_code_array);
void destory_code_array_type(struct code_array_type *p_code_array);

void image_encode_proccess(int sockfd);
void image_decode_proccess(int sockfd);

#endif //FLY_BIGGER_SCREEN_IMAGECODE_H
