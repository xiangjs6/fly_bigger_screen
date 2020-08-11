//
// Created by xjs on 2020/7/28.
//

#include "ImageMesh.h"

int initMesh(MeshHead *h_mesh, Rect size, Rect mesh_size)
{
    h_mesh->size = size;
    h_mesh->mesh_size = mesh_size;
    if((h_mesh->mesh = malloc(sizeof(Mesh) * RECT_LENGTH(size))) == NULL)
        return -1;
    if((h_mesh->image_buff = malloc(PIXEL_LENGTH(RECT_LENGTH(size) * RECT_LENGTH(mesh_size)))) == NULL) {
        free(h_mesh->mesh);
        return -1;
    }
    return 0;
}

void destoryMesh(MeshHead *h_mesh)
{
    if(!h_mesh)
        return;
    free(h_mesh->mesh);
    free(h_mesh->image_buff);
}

int initShareMesh(MeshHead *h_mesh, Rect size, Rect mesh_size)
{
    h_mesh->size = size;
    h_mesh->mesh_size = mesh_size;
    if((h_mesh->mesh = shareMalloc(sizeof(Mesh) * RECT_LENGTH(size), ANONYMOUS_KEY)) == NULL)
        return -1;
    if((h_mesh->image_buff = shareMalloc(PIXEL_LENGTH(RECT_LENGTH(size) * RECT_LENGTH(mesh_size)), ANONYMOUS_KEY)) == NULL) {
        shareFree(h_mesh->mesh);
        return -1;
    }
    return 0;
}

void destoryShareMesh(MeshHead *h_mesh)
{
    if(!h_mesh)
        return;
    shareFree(h_mesh->mesh);
    shareFree(h_mesh->image_buff);
}

Mesh *getMeshHead(MeshHead *h_mesh, int row, int col)
{
    if(row < 0 || col < 0)
        return NULL;
    if(row >= h_mesh->size.height || col >= h_mesh->size.width)
        return NULL;
    return &h_mesh->mesh[h_mesh->size.width * row + col];
}

void putMeshHead(MeshHead *h_mesh, int row, int col, Mesh in_mesh)
{
    if(row < 0 || col < 0)
        return;
    if(row >= h_mesh->size.height || col >= h_mesh->size.width)
        return;
    h_mesh->mesh[h_mesh->size.width * row + col] = in_mesh;
}

int ImagetoMesh(MeshHead dst_mesh, PImage src_image) {
    Rect mesh_size = dst_mesh.mesh_size;
    PPixel image_buff = dst_mesh.image_buff;
    if (src_image.size.height % mesh_size.height || src_image.size.width % mesh_size.width)
        return -1;
    Rect mesh_arr_size = {src_image.size.width / mesh_size.width, src_image.size.height / mesh_size.height};
    Mesh *p_mesh = dst_mesh.mesh;
    int mesh_pixel_len = RECT_LENGTH(mesh_size);
    for (int row = 0; row < mesh_arr_size.height; row++) {
        for (int col = 0; col < mesh_arr_size.width; col++) {
            Point src_point = {col * mesh_size.width, row * mesh_size.height};
            p_mesh->image.size = mesh_size;
            p_mesh->point = src_point;
            p_mesh->image.data = image_buff;
            image_buff += mesh_pixel_len;
            imageCopy(p_mesh->image, src_image, ORIGIN_POINT, src_point, mesh_size);
            p_mesh++;
        }
    }
    return 0;
}

int MeshtoImage(MeshHead src_mesh, PImage dst_image)
{
    Rect image_size = {.width = src_mesh.size.width * src_mesh.mesh_size.width,
                       .height = src_mesh.size.height * src_mesh.mesh_size.height};
    if (dst_image.size.width < image_size.width || dst_image.size.height < image_size.height)
        return -1;
    dst_image.size = image_size;
    for (int row = 0; row < src_mesh.size.height; row++) {
        for (int col = 0; col < src_mesh.size.width; col++) {
            Mesh *mesh = getMeshHead(&src_mesh, row, col);
            imageCopy(dst_image, mesh->image, mesh->point, ORIGIN_POINT, mesh->image.size);
        }
    }
    return 0;
}

void allocMeshFromBuff(MeshHead *h_mesh)
{
    Mesh *p_mesh = h_mesh->mesh;
    PPixel image_buff = h_mesh->image_buff;
    int mesh_pixel_len = RECT_LENGTH(h_mesh->mesh_size);

    for (int i = 0; i < h_mesh->size.height; i++) {
        for (int j = 0; j < h_mesh->size.width; j++) {
            Point src_point = {j * h_mesh->mesh_size.width, i * h_mesh->mesh_size.height};
            p_mesh->point = src_point;
            p_mesh->image.size = h_mesh->mesh_size;
            p_mesh->image.data = image_buff;
            image_buff += mesh_pixel_len;
            p_mesh++;
        }
    }
}