//
// Created by xjs on 2020/8/6.
//

#include "ImageEncode.h"
#include "ImageCode.h"
#include "PImage.h"
#include "../evdi/evdi.h"
#include "ImageMesh.h"
#include "ImageHash.h"
#include "../protocols/program_protocol.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <memory.h>
#include <sys/timeb.h>
#include <time.h>

static int init(void);
static void destory(void);
static void request_image(long int us);

extern LoopArrayOpts code_opts;

extern Rect mesh_num_size;
extern Rect mesh_size;

static PPixelBGRA evdi_buff;
static PImage screen_image;
static struct code_array_type code_element;
static MeshHead *next_mesh;
static LoopArrayHead loop_array;
static ImageHashMap image_map;

static struct pyramid_code *pyramids_link = NULL;

void image_encode_proccess(int sockfd)
{
    if(init() < 0)
        exit(-1);
    init_evdi(evdi_buff);
    struct program_protocol message;
    struct encode_response_protocol *response;

    time_t ltime1, ltime2, tmp_time;
    struct timeb tstruct1, tstruct2;
    bool need_send = false;//用于标志是否还有数据需要继续传输

    while (true)
    {
        if (read(sockfd, &message, sizeof(message)) != sizeof(message)) {
            printf("1\n");
            break;
        }
        if (message.protocol_label != REQUST_ENCODE_IMAGE)
            continue;

        if (need_send)
            request_image(1000);
        else
            request_image(-1);

        need_send = false;

        memset(code_element.mesh_updata_mark, NOUPDATA, sizeof(char) * RECT_LENGTH(mesh_num_size));
        ImagetoMesh(*next_mesh, screen_image);

        ImageVal *mesh_mark = shareMalloc(RECT_LENGTH(next_mesh->size) * sizeof(ImageVal), AUTO_KEY);
        //ImageVal mesh_mark[next_mesh->size.height][next_mesh->size.width];
        memset(mesh_mark, -1, sizeof(RECT_LENGTH(next_mesh->size)) * sizeof(ImageVal));

        struct code_array_type old_code_element = code_element;
        MeshHead old_next_mesh = *next_mesh;

        code_element.is_used = true;
        int array_index = pushLoopArray(&loop_array, (LoopArrayDataType) {.p_val = &code_element});//将新数据和老数据进行了交换

        if (code_element.is_used) {
            for (int i = 0; i < next_mesh->size.height; i++) {
                for (int j = 0; j < next_mesh->size.width; j++) {
                    Mesh *mesh;
                    struct pyramid_code *node;

                    //删除节点
                    node = code_element.pyramid_trees[mesh_num_size.width * i + j];
                    unlinkNode(node);
                    if (!node->link_count)
                        del_pyramid_node(&pyramids_link, node);
                    code_element.pyramid_trees[mesh_num_size.width * i + j] = NULL;

                    switch (code_element.mesh_updata_mark[mesh_num_size.width * i + j])
                    {
                        case UPDATA:
                            code_element.pyramid_trees[mesh_num_size.width * i + j] = NULL;
                            break;
                        case NOUPDATA:
                            mesh = getMeshHead(next_mesh, i, j);
                            ImageKey key = {.key = mesh->image.data, .len = PIXEL_LENGTH(RECT_LENGTH(mesh->image.size))};
                            if (delImageHashMap(&image_map, key) < 0) {
                                printf("2");
                                return;
                            }
                            break;
                    }
                }
            }
        }//if(new_array_type.is_used)

        //查看相同图片块
/*        for (int i = 0; i < old_next_mesh.size.height; i++) {
            for (int j = 0; j < old_next_mesh.size.width; j++) {
                Mesh *mesh = getMeshHead(&old_next_mesh, i, j);
                ImageKey key = {.key = mesh->image.data, .len = PIXEL_LENGTH(RECT_LENGTH(mesh->image.size))};
                ImageVal val = {.index = array_index, .h_mesh_point = (Point) {.x = j, .y = i}};
                int res = getImageHashMap(&image_map, key, &val);
                //int res = putImageHashMap(&image_map, key, val, &old_val);
                if (res < 0) {
                    printf("3");
                    return;
                }
                mesh_mark[i * old_next_mesh.size.width + j] = val;
            }
        }*/

        for (int i = 0; i < old_next_mesh.size.height; i++) {
            for (int j = 0; j < old_next_mesh.size.width; j++) {
                Mesh *mesh = getMeshHead(&old_next_mesh, i, j);
                ImageKey key = {.key = mesh->image.data, .len = PIXEL_LENGTH(RECT_LENGTH(mesh->image.size))};
                ImageVal val = {.index = array_index, .h_mesh_point = (Point) {.x = j, .y = i}};
                ImageVal old_val = val; //这个old_val用来得到前一个ImageVal的值，并将前网格一个置为UPDATA
                int res = putImageHashMap(&image_map, key, val, &old_val);

                mesh_mark[i * old_next_mesh.size.width + j] = old_val;

                int old_index = mesh_num_size.width * old_val.h_mesh_point.y + old_val.h_mesh_point.x;
                int index = mesh_num_size.width * i + j;
                if (res == 0 /*|| array_index == mesh_mark[index].index*/) { //如果一次屏幕，则为每个图片都创建一个图像金字塔，最终只会留下一个
                    //创建节点，并入链表
                    struct pyramid_code *node = creat_pyramid_node(&pyramids_link);
                    old_code_element.pyramid_trees[index] = linkNode(node);
                    //生成图像金字塔
                    imagePyramid(&node->tree, mesh->image);
                    /*if (res == 1) {
                        struct code_array_type *old_element = getLoopArray(&loop_array, old_val.index).p_val;
                        old_element->mesh_updata_mark[old_index] = SAME_SCREEN;
                    }*/
                } else if(res == 1) {
                    struct code_array_type *old_element = getLoopArray(&loop_array, old_val.index).p_val;
                    old_code_element.pyramid_trees[index] = linkNode(old_element->pyramid_trees[old_index]);
                    old_element->mesh_updata_mark[old_index] = UPDATA;
                } else {
                    printf("4");
                    return;
                }
            }
        }

        //准备传输数据
        ImagePyrDataType **pyramids_array = shareMalloc(sizeof(ImagePyrDataType*) * RECT_LENGTH(mesh_num_size), AUTO_KEY);
        if (!pyramids_array) {
            printf("5");
            break;
        }
        for (int i = 0; i < mesh_num_size.height; i++) {
            for (int j = 0; j < mesh_num_size.width; j++) {
                int index = i * mesh_num_size.width + j;
                if (!old_code_element.pyramid_trees[index]->tree.stack.size) {
                    pyramids_array[index] = NULL;
                    continue;
                }
                ImagePyrDataType *p = getStack(&old_code_element.pyramid_trees[index]->tree.stack, STACK_TOP)->p_val;
                pyramids_array[index] = shareMalloc(sizeof(ImagePyrDataType), ANONYMOUS_KEY);
                if (!pyramids_array[index]) {
                    printf("6");
                    return;
                }

                *pyramids_array[index] = *p;
                pyramids_array[index]->image.data = shareMalloc(PIXEL_LENGTH(RECT_LENGTH(p->image.size)), ANONYMOUS_KEY);
                if (!pyramids_array[index]->image.data) {
                    printf("7");
                    return;
                }

                StackDataType data;
                data.p_val = pyramids_array[index];
                popStack(&old_code_element.pyramid_trees[index]->tree.stack, &data);
                if (old_code_element.pyramid_trees[index]->tree.stack.size)
                    need_send = true;
            }
        }

        message.protocol_label = RESPONSE_ENCODE_IMAGE;
        response = &message.response_encode;
        response->seq = message.requst_encode.seq;
        response->mesh_mark_key = getShareKey(mesh_mark);
        response->mesh_num_size = old_next_mesh.size;
        //response->mesh_head_array_key = getShareKey(mesh_head_array);
        //response->mesh_size = mesh_size;
        response->pyramids_key = getShareKey(pyramids_array);
        response->curent_array_index = array_index;



        write(sockfd, &message, sizeof(message));

        //sleep(1);
        /*if(count == 0)
            break;*/
    }//while (read(sockfd, message, sizeof(message)) > 0)
    destory();
}

//1280*720    16:9
static int init(void)
{
    mesh_num_size = (Rect){40, 45};
    mesh_size = (Rect){40, 20};
    pyramids_link = NULL;

    screen_image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(((Rect){1600, 900}))));
    if(!screen_image.data)
        return -1;
    screen_image.size = (Rect){1600, 900};

    evdi_buff = malloc(PIXEL_LENGTHBGRA(RECT_LENGTH(screen_image.size)));
    if (!evdi_buff)
        return -1;

    if(init_code_array_type(&code_element) < 0)
        return -1;
    next_mesh = code_element.h_mesh;

    loop_array = creatLoopArray(60, code_opts, NULL);
    if(!loop_array.array)
        return -1;

    image_map = craetImageHashMap(4096);
    if(!image_map.table)
        return -1;
    return 0;
}

static void destory(void)
{
    free(screen_image.data);
    free(evdi_buff);
    destory_code_array_type(&code_element);
    destoryLoopArray(&loop_array);
    //shareFree(loop_array.array);
    destoryImageHashMap(&image_map);
}

static void request_image(long int us)
{
    get_screen(us);
    BGRA2BGR(evdi_buff, screen_image.data, RECT_LENGTH(screen_image.size));
/*    char s[1024];
    sprintf(s, "/home/xjs/screen_picture/%d", count);
    int fd = open(s, O_RDONLY);
    int n = read(fd, screen_image.data, PIXEL_LENGTH(RECT_LENGTH(screen_image.size)));
    //printf("screen id size:%d %d\n", count, n);
    count = (count + 1) % 17;
    close(fd);
    //memset(screen_image.data, -1, PIXEL_LENGTH(RECT_LENGTH(screen_image.size)));*/
}
