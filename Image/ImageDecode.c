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
static MeshHead *next_mesh;
static LoopArrayHead loop_array;

static int init(void);
static void destory(void);

void image_decode_proccess(int sockfd)
{
    struct program_protocol message;
    struct decode_requst_protocol *requst;
    int trans_len;
    uint32_t seq = 0;
    if (init() < 0)
        return;

    time_t ltime1, ltime2, tmp_time;
    struct timeb tstruct1, tstruct2;

    while (true)
    {
        ftime (&tstruct1);
        time (&ltime1);

        ImageVal *mesh_mark = shareMalloc(RECT_LENGTH(next_mesh->size) * sizeof(ImageVal), AUTO_KEY);
        memset(mesh_mark, -1, sizeof(RECT_LENGTH(next_mesh->size)) * sizeof(ImageVal));
        allocMeshFromBuff(next_mesh);
        //发出请求
        message.protocol_label = REQUST_DECODE_IMAGE;
        requst = &message.requst_decode;
        requst->seq = seq;
        requst->mesh_num_size = mesh_num_size;
        requst->mesh_size = mesh_size;
        requst->mesh_head_key = getShareKey(next_mesh);
        requst->mesh_mark_key = getShareKey(mesh_mark);
        //接受请求
        if (write(sockfd, &message, sizeof(message)) < 0)
            break;
        if ((trans_len = read(sockfd, &message, sizeof(message))) < 0)
            break;
        if (trans_len != sizeof(message) || message.protocol_label != RESPONSE_DECODE_IMAGE)
            continue;
        //拼接图片
        for (int i = 0; i < mesh_num_size.height; i++) {
            for (int j = 0; j < mesh_num_size.width; j++) {
                int index = i * mesh_num_size.width + j;
                if (seq % loop_array.size == mesh_mark[index].index)
                    continue;
                struct code_array_type *old_element = getLoopArray(&loop_array, mesh_mark[index].index).p_val;
                Mesh *curent_mesh = getMeshHead(next_mesh, i, j);
                Mesh *old_mesh = getMeshHead(old_element->h_mesh,
                                             mesh_mark[index].h_mesh_point.y,
                                             mesh_mark[index].h_mesh_point.x);
                curent_mesh->point = (Point){.x = mesh_size.width * j, .y = mesh_size.height * i};
                imageCopy(curent_mesh->image, old_mesh->image, ORIGIN_POINT, ORIGIN_POINT, mesh_size);
            }
        }
        MeshtoImage(*next_mesh, screen_image);

        time (&ltime2);               // end time sec
        ftime (&tstruct2);
        tmp_time = (ltime2 * 1000 + tstruct2.millitm) - (ltime1 * 1000 + tstruct1.millitm);
        printf("The difference is: %ld millitm\n",tmp_time);

        //显示
        showImage(screen_image.data, RECT_LENGTH(screen_image.size));
        pushLoopArray(&loop_array, (LoopArrayDataType) {.p_val = &code_element});
        seq++;
        shareFree(mesh_mark);
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
    next_mesh = code_element.h_mesh;
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