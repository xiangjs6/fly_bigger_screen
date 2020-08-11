//
// Created by xjs on 2020/7/28.
//

#ifndef FLY_BIGGER_SCREEN_IMAGEMESH_H
#define FLY_BIGGER_SCREEN_IMAGEMESH_H

#include "PImage.h"
#include "../Util/Util.h"
#include <stdlib.h>
typedef struct {
    PImage image;
    Point point;
} Mesh;

typedef struct {
    Rect size;
    Rect mesh_size;
    PPixel image_buff;
    Mesh *mesh;
} MeshHead;

int initMesh(MeshHead *h_mesh, Rect size, Rect mesh_size);
void destoryMesh(MeshHead *h_mesh);
int initShareMesh(MeshHead *h_mesh, Rect size, Rect mesh_size);
void destoryShareMesh(MeshHead *h_mesh);
int ImagetoMesh(MeshHead dst_mesh, PImage src_image);
int MeshtoImage(MeshHead src_mesh, PImage dst_image);
Mesh *getMeshHead(MeshHead *h_mesh, int row, int col);
void putMeshHead(MeshHead *h_mesh, int row, int col, Mesh in_mesh);
void allocMeshFromBuff(MeshHead *h_mesh);
#endif //FLY_BIGGER_SCREEN_IMAGEMESH_H
