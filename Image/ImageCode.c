//
// Created by xjs on 2020/7/27.
//

#include "ImageCode.h"
#include "PImage.h"
#include "ImageMesh.h"
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include "../evdi/evdi.h"

static void code_copy(LoopArrayDataType *src, LoopArrayDataType *dst);
static int code_malloc(LoopArrayDataType *data);
static void code_free(LoopArrayDataType *data);
LoopArrayOpts code_opts = {.malloc = code_malloc, .free = code_free, .copy = code_copy};

Rect mesh_num_size;
Rect mesh_size;

int init_code_array_type(struct code_array_type *p_code_array)
{
    p_code_array->is_used = false;
    //MeshHead *p_mesh = p_code_array->h_mesh = shareMalloc(sizeof(MeshHead), AUTO_KEY);
    MeshHead *p_mesh = p_code_array->h_mesh = malloc(sizeof(MeshHead));
    if(!p_mesh)
        return -1;
    /*if(initShareMesh(p_mesh, mesh_num_size, mesh_size) < 0)
        return -1;*/
    if(initMesh(p_mesh, mesh_num_size, mesh_size) < 0)
        return -1;
    p_code_array->mesh_updata_mark = malloc(sizeof(char) * RECT_LENGTH(p_mesh->size));
    if(!p_code_array->mesh_updata_mark)
        return -1;
    memset(p_code_array->mesh_updata_mark, NOUPDATA, sizeof(char) * RECT_LENGTH(p_mesh->size));
    p_code_array->pyramid_trees = malloc(RECT_LENGTH(p_mesh->size) * sizeof(struct pyramid_code*));
    if (!p_code_array->pyramid_trees)
        return -1;
    memset(p_code_array->pyramid_trees, 0, RECT_LENGTH(p_mesh->size) * sizeof(struct pyramid_code*));
    return 0;
}

void destory_code_array_type(struct code_array_type *p_code_array)
{
    if(!p_code_array)
        return;
    MeshHead *p_mesh = p_code_array->h_mesh;
    //destoryShareMesh(p_mesh);
    destoryMesh(p_mesh);
    //shareFree(p_mesh);
    free(p_mesh);
    free(p_code_array->mesh_updata_mark);
    free(p_code_array->pyramid_trees);
}

struct pyramid_code *creat_pyramid_node(struct pyramid_code **head)
{
    struct pyramid_code *node = malloc(sizeof(struct pyramid_code));
    if (!node)
        return node;
    node->next = *head;
    node->link_count = 0;
    node->tree = initImagePyrTree(1);
    node->pre = NULL;
    if (*head)
        (*head)->pre = node;
    *head = node;
    return node;
}

void del_pyramid_node(struct pyramid_code **head, struct pyramid_code *node)
{
    if (node->next)
        node->next->pre = node->pre;
    if (!node->pre)
        *head = node->next;
    else
        node->pre->next = node->next;
    destoryImagePyrTree(&node->tree);
    free(node);
}

struct pyramid_code *linkNode(struct pyramid_code *node)
{
    node->link_count++;
    return node;
}

void unlinkNode(struct pyramid_code *node)
{
    node->link_count--;
}

static void code_copy(LoopArrayDataType *src, LoopArrayDataType *dst)
{
    //和外面交换内存地址，外面内存也必须是共享内存的地址。以后可能得改改
    struct code_array_type *src_code_array = src->p_val, *dst_code_array = dst->p_val;
    MeshHead *src_mesh = src_code_array->h_mesh, *dst_mesh = dst_code_array->h_mesh;
    SWAP(*src_mesh, *dst_mesh);
    SWAP(src_code_array->mesh_updata_mark, dst_code_array->mesh_updata_mark);
    SWAP(src_code_array->is_used, dst_code_array->is_used);
    SWAP(src_code_array->pyramid_trees, dst_code_array->pyramid_trees);
}

static int code_malloc(LoopArrayDataType *data)
{
    struct code_array_type *p_code_array = data->p_val = malloc(sizeof(struct code_array_type));
    if(!p_code_array)
        return -1;
    return init_code_array_type(p_code_array);
}

static void code_free(LoopArrayDataType *data)
{
    struct code_array_type *p_code_array = data->p_val;
    destory_code_array_type(p_code_array);
    free(p_code_array);
}
