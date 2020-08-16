//
// Created by xjs on 2020/8/6.
//

#include "ImageDecode.h"
#include "ImageCode.h"
#include "PImage.h"
#include "ImageMesh.h"
#include "ImageHash.h"
#include "../protocols/program_protocol.h"
#include "../debug.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>

#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>

extern LoopArrayOpts code_opts;
extern Rect mesh_num_size;
extern Rect mesh_size;

static PImage screen_image;
static struct code_array_type code_element;
static LoopArrayHead loop_array;
static struct pyramid_code *pyramids_link = NULL;

static int init(void);
static void destory(void);

static int requst_image(int seq, int fd);

void image_decode_proccess(int sockfd)
{
    struct program_protocol message;
    //struct decode_requst_protocol *requst;
    int trans_len;
    uint32_t seq = 0;
    if (init() < 0)
        return;

    time_t ltime1, ltime2, tmp_time;
    struct timeb tstruct1, tstruct2;

    if (requst_image(seq++, sockfd) < 0)
        return;
    while (true)
    {

        MeshHead *next_mesh = code_element.h_mesh;
        /*ImageVal *mesh_mark = shareMalloc(RECT_LENGTH(next_mesh->size) * sizeof(ImageVal), AUTO_KEY);
        memset(mesh_mark, -1, sizeof(RECT_LENGTH(next_mesh->size)) * sizeof(ImageVal));
        ImagePyrDataType **pyramids_array = shareMalloc(sizeof(ImagePyrDataType*) * RECT_LENGTH(mesh_num_size), AUTO_KEY);
        memset(pyramids_array, NULL, sizeof(ImagePyrDataType*) * RECT_LENGTH(mesh_num_size));*/
        allocMeshFromBuff(next_mesh);
        //发出请求
        /*message.protocol_label = REQUST_DECODE_IMAGE;
        requst = &message.requst_decode;
        requst->seq = seq;
        requst->mesh_num_size = mesh_num_size;
        //requst->mesh_size = mesh_size;
        requst->pyramids_key = getShareKey(pyramids_array);
        //requst->mesh_head_key = getShareKey(next_mesh);
        requst->mesh_mark_key = getShareKey(mesh_mark);
        //发送请求
        if (write(sockfd, &message, sizeof(message)) < 0)
            break;*/
        if ((trans_len = read(sockfd, &message, sizeof(message))) < 0)
            break;
        if (trans_len != sizeof(message) || message.protocol_label != RESPONSE_DECODE_IMAGE)
            continue;
        int response_seq = message.response_decode.seq;
        //再次请求
        if (requst_image(seq++, sockfd) < 0)
            break;

        ImageVal *mesh_mark = getShareMemory(message.response_decode.mesh_mark_key);
        ImagePyrDataType **pyramids_array = getShareMemory(message.response_decode.pyramids_key);
        memset(code_element.mesh_updata_mark, NOUPDATA, sizeof(char) * RECT_LENGTH(mesh_num_size));
        //拼接图片
        for (int i = 0; i < mesh_num_size.height; i++) {
            for (int j = 0; j < mesh_num_size.width; j++) {
                int index = i * mesh_num_size.width + j;
                int old_index = mesh_mark[index].h_mesh_point.y * mesh_num_size.width + mesh_mark[index].h_mesh_point.x;
                if (pyramids_array[index] == NULL) {
                    //已经完成传输的节点
                    struct code_array_type *old_element = getLoopArray(&loop_array, mesh_mark[index].index).p_val;
                    Mesh *curent_mesh = getMeshHead(next_mesh, i, j);
                    /*Mesh *old_mesh = getMeshHead(old_element->h_mesh,
                                                 mesh_mark[index].h_mesh_point.y,
                                                 mesh_mark[index].h_mesh_point.x);*/
                    curent_mesh->point = (Point){.x = mesh_size.width * j, .y = mesh_size.height * i};
                    //imageCopy(curent_mesh->image, old_mesh->image, ORIGIN_POINT, ORIGIN_POINT, mesh_size);
                    //将金字塔节点关联到新的数组元素中;
                    code_element.pyramid_trees[index] = linkNode(old_element->pyramid_trees[old_index]);
                    imageResize(code_element.pyramid_trees[index]->tree.max_size_pyramid->image, curent_mesh->image, mesh_size);
                    old_element->mesh_updata_mark[old_index] = UPDATA;
                    //old_element->pyramid_trees[old_index] = NULL;
                } else {
                    Mesh *curent_mesh = getMeshHead(next_mesh, i, j);
                    struct pyramid_code *node = NULL;
                    if (response_seq % loop_array.size == mesh_mark[index].index) { //新节点
                        node = creat_pyramid_node(&pyramids_link);
                    } else { //增量节点
                        struct code_array_type *old_element = getLoopArray(&loop_array, mesh_mark[index].index).p_val;
                        node = old_element->pyramid_trees[old_index];
                        old_element->mesh_updata_mark[old_index] = UPDATA;
                        //old_element->pyramid_trees[old_index] = NULL;
                    }
                    if (putPyramid(&node->tree, *pyramids_array[index]) < 0)
                        return;
                    PImage a = node->tree.max_size_pyramid->image;
                    PImage b = curent_mesh->image;
                    imageResize(node->tree.max_size_pyramid->image, curent_mesh->image, mesh_size);
                    code_element.pyramid_trees[index] = linkNode(node);

                    shareFree(pyramids_array[index]->image.data);
                    shareFree(pyramids_array[index]);
                }
/*                //加入金字塔编码之前
                if (seq % loop_array.size == mesh_mark[index].index)
                    continue;
                struct code_array_type *old_element = getLoopArray(&loop_array, mesh_mark[index].index).p_val;
                Mesh *curent_mesh = getMeshHead(next_mesh, i, j);
                Mesh *old_mesh = getMeshHead(old_element->h_mesh,
                                             mesh_mark[index].h_mesh_point.y,
                                             mesh_mark[index].h_mesh_point.x);
                curent_mesh->point = (Point){.x = mesh_size.width * j, .y = mesh_size.height * i};
                imageCopy(curent_mesh->image, old_mesh->image, ORIGIN_POINT, ORIGIN_POINT, mesh_size);*/
            }
        }

        MeshtoImage(*next_mesh, screen_image);

        time (&ltime2);               // end time sec
        ftime (&tstruct2);
        tmp_time = (ltime2 * 1000 + tstruct2.millitm) - (ltime1 * 1000 + tstruct1.millitm);
        printf("The difference is: %ld millitm\n",tmp_time);

        ftime (&tstruct1);
        time (&ltime1);

        //显示
        showImage(screen_image.data, RECT_LENGTH(screen_image.size));

        code_element.is_used = true;
        pushLoopArray(&loop_array, (LoopArrayDataType) {.p_val = &code_element});
        if (code_element.is_used) {
            for (int i = 0; i < mesh_num_size.height; i++) {
                for (int j = 0; j < mesh_num_size.width; j++) {
                    int index = i * mesh_num_size.width + j;
                    unlinkNode(code_element.pyramid_trees[index]);
                    if (!code_element.pyramid_trees[index]->link_count)
                        del_pyramid_node(&pyramids_link, code_element.pyramid_trees[index]);
                    code_element.pyramid_trees[index] = NULL;
                }
            }
        }
        //seq++;
        shareFree(mesh_mark);
        shareFree(pyramids_array);
        //usleep(16666);
        //sleep(1);
    }
    destory();
}

static int init(void)
{
    mesh_num_size = (Rect){40, 45};
    mesh_size = (Rect){32, 16};
    if(init_code_array_type(&code_element) < 0)
        return -1;

    loop_array = creatLoopArray(60, code_opts, NULL);
    if(!loop_array.array)
        return -1;
    screen_image.size = (Rect){1280, 720};
    screen_image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(screen_image.size)));
    if (!screen_image.data)
        return -1;
    return 0;
}

static void destory(void)
{
    destory_code_array_type(&code_element);
    destoryLoopArray(&loop_array);
    free(screen_image.data);
}

static int requst_image(int seq, int fd)
{
    struct program_protocol message;
    message.protocol_label = REQUST_DECODE_IMAGE;

    ImageVal *mesh_mark = shareMalloc(RECT_LENGTH(mesh_num_size) * sizeof(ImageVal), AUTO_KEY);
    if (!mesh_mark)
        return -1;
    memset(mesh_mark, -1, sizeof(RECT_LENGTH(mesh_num_size)) * sizeof(ImageVal));
    ImagePyrDataType **pyramids_array = shareMalloc(sizeof(ImagePyrDataType*) * RECT_LENGTH(mesh_num_size), AUTO_KEY);
    if (!pyramids_array) {
        shareFree(mesh_mark);
        return -1;
    }
    memset(pyramids_array, 0, sizeof(ImagePyrDataType*) * RECT_LENGTH(mesh_num_size));

    struct decode_requst_protocol *requst = &message.requst_decode;
    requst->seq = seq;
    requst->mesh_num_size = mesh_num_size;
    //requst->mesh_size = mesh_size;
    requst->pyramids_key = getShareKey(pyramids_array);
    //requst->mesh_head_key = getShareKey(next_mesh);
    requst->mesh_mark_key = getShareKey(mesh_mark);
    return write(fd, (char*)&message, sizeof(message));
}