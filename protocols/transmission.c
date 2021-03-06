//
// Created by xjs on 2020/8/3.
//

#include <stdbool.h>
#include <unistd.h>
#include <net/if.h>
#include <memory.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "transmission.h"
#include "program_protocol.h"
#include "../Image/ImageMesh.h"
#include "protocols.h"
#include "../Image/ImagePyrTree.h"

#define PORT 1234
#define MESSAGE_LEN 1024 * 1024

static int init_sever_socket(void)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock_fd, 2);
    int fd = accept(sock_fd, NULL, NULL);
    close(sock_fd);
    return fd;
}

static int init_client_socket(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    return fd;
}

static int force_read(int fd, char *buf, size_t len)
{
    int n_len = 0;
    while (n_len != len) {
        int n = read(fd, buf + n_len, len - n_len);
        if (n < 0)
            return -1;
        n_len += n;
    }
    return len;
}

static int recv_net_requst_image(int fd, image_requst_protocol *image_requst_message)
{
    char message[MESSAGE_LEN];
    int n;
    if ((n = force_read(fd, message, IMAGE_REQUST_HEAD_SIZE)) != IMAGE_REQUST_HEAD_SIZE)
        return -1;
    if (net_to_image_requst(message, n, image_requst_message) < 0)
        return -1;
    return n;
}

static int send_net_response_image(int fd, image_response_protocol *image_response_message)
{
    enum label_type label = RESPONSE_IMAGE;
    char message[MESSAGE_LEN];
    int n_len = 0;
    int n;
    if ((n = putLabelToNet(label, message, MESSAGE_LEN)) < 0)
        return -1;
    n_len += n;
    if((n = image_response_head_to_net(image_response_message, message + n_len, MESSAGE_LEN - n_len)) < 0)
        return -1;
    n_len += n;
    if (image_response_message->head.len != 0) {
        memcpy(message + n_len, image_response_message->data, image_response_message->head.len);
        n_len += image_response_message->head.len;
    }
    if (write(fd, message, n_len) < 0)
        return -1;
    return n_len;
}

static int send_net_requst_image(int fd, image_requst_protocol *image_requst_message)
{
    char message[MESSAGE_LEN];
    enum label_type label = REQUST_IMAGE;
    int n_len = 0;
    int n;
    if ((n = putLabelToNet(label, message, MESSAGE_LEN)) < 0)
        return -1;
    n_len += n;
    if ((n = image_requst_to_net(image_requst_message, message + n_len, MESSAGE_LEN - n_len)) < 0)
        return -1;
    n_len += n;
    if (write(fd, message, n_len) != n_len)
        return -1;
    return n_len;
}

static int recv_net_response_image(int fd, image_response_protocol *image_response_message)
{
    char message[MESSAGE_LEN];
    int n_len = 0;
    int n;

    if ((n = force_read(fd, message, IMAGE_RESPONSE_HEAD_SIZE)) < 0) {
        return -1;
    }
    net_to_image_head_response(message, n, image_response_message);
    n_len += n;
    return n_len;
}

static enum label_type recv_net_label(int fd)
{
    char message[LABEL_SIZE];
    enum label_type label;
    int n;
    if ((n = force_read(fd, message, LABEL_SIZE)) != LABEL_SIZE) {
        return -1;
    }
    if (getLabelFromNet(message, n, &label) < 0)
        return -1;
    return label;
}

void sever_transmission_proccess(int sockfd)
{
    int client_fd = init_sever_socket();

    struct program_protocol program_message_header;
    image_requst_protocol image_requst_message;
    image_response_protocol image_response_message;
    struct encode_response_protocol encode_response_message;
    enum label_type label;
    //int trans_len;

    char *message = malloc(MESSAGE_LEN);
    if (!message)
        return;
    //图像传输
    while (true)
    {
        if ((label = recv_net_label(client_fd)) < 0)
            break;
        /*if ((trans_len = force_read(client_fd, message, LABEL_SIZE)) != LABEL_SIZE)
            break;
        getLabelFromNet(message, trans_len, &label);*/

        if (label != REQUST_IMAGE)
            continue;
        if (recv_net_requst_image(client_fd, &image_requst_message) < 0)
            break;
        /*if ((trans_len = force_read(client_fd, message, IMAGE_REQUST_HEAD_SIZE)) != IMAGE_REQUST_HEAD_SIZE)
            break;
        net_to_image_requst(message, trans_len, &image_requst_message);*/

        program_message_header.protocol_label = REQUST_ENCODE_IMAGE;
        program_message_header.requst_encode.seq = image_requst_message.seq;

        if (write(sockfd, &program_message_header, sizeof(program_message_header)) != sizeof(program_message_header))
            break;
        if (force_read(sockfd, (char*)&program_message_header, sizeof(program_message_header)) < 0)
            break;

        encode_response_message = program_message_header.response_encode;

        //MeshHead **mesh_head_array = getShareMemory(encode_response_message.mesh_head_array_key);
        ImagePyrDataType **pyramids_array = getShareMemory(encode_response_message.pyramids_key);
        ImageVal *mesh_mark = getShareMemory(encode_response_message.mesh_mark_key);
        Rect mesh_num_size = encode_response_message.mesh_num_size;
        //Rect mesh_size = encode_response_message.mesh_size;
        if(!mesh_mark || !pyramids_array)
            break;

        image_response_message.head.seq = encode_response_message.seq;
        for (int i = 0; i < mesh_num_size.height; i++) {
            for (int j = 0; j < mesh_num_size.width; j++) {
                int index = i * mesh_num_size.width + j;
                image_response_message.head.point = (Point) {.x = j, .y = i};
                image_response_message.head.index_val = mesh_mark[index];
                if (pyramids_array[index]) {
                    image_response_message.head.image_size = pyramids_array[index]->image.size;
                    if (mesh_mark[index].index != encode_response_message.curent_array_index)
                        image_response_message.head.type = INCRESS;
                    else
                        image_response_message.head.type = NEW;
                    image_response_message.head.layer = pyramids_array[index]->node_layer;
                    image_response_message.data = pyramids_array[index]->image.data;
                    image_response_message.head.len = PIXEL_LENGTHBGR(RECT_LENGTH(pyramids_array[index]->image.size));
                } else {
                    image_response_message.head.image_size = (Rect){0, 0};
                    image_response_message.head.type = OLD;
                    image_response_message.head.len = 0;
                    image_response_message.head.layer = -1;
                    image_response_message.data = NULL;
                }

/*                //加图像金字塔之前
                if(mesh_mark[index].index != encode_response_message.curent_array_index) {
                    image_response_message.head.len = 0;
                    image_response_message.head.type = OLD;
                    //printf("sever_transmission_proccess OLD %d %d\n", image_response_message.head.type, OLD);
                } else {
                    image_response_message.head.len = PIXEL_LENGTHBGR(RECT_LENGTH(mesh_size));
                    Mesh old_mesh = *getMeshHead(mesh_head_array[mesh_mark[index].index],
                                                 mesh_mark[index].h_mesh_point.y,
                                                 mesh_mark[index].h_mesh_point.x);
                    image_response_message.data = old_mesh.image.data;
                    image_response_message.head.type = NEW;
                    //printf("sever_transmission_proccess NEW %d %d\n", image_response_message.head.type, NEW);
                }*/

                if (send_net_response_image(client_fd, &image_response_message) < 0)
                    return;
                /*label = RESPONSE_IMAGE;
                int n_len = 0;
                int n;
                if ((n = putLabelToNet(label, message, MESSAGE_LEN)) < 0)
                    return;
                n_len += n;
                if((n = image_response_head_to_net(&image_response_message, message + n_len, MESSAGE_LEN - n_len)) < 0)
                    return;
                n_len += n;
                if (image_response_message.head.len != 0) {
                    memcpy(message + n_len, image_response_message.data, image_response_message.head.len);
                    shareFree(pyramids_array[index]->image.data);
                    n_len += image_response_message.head.len;
                }
                //static int count = 0;
                //printf("%d\n", n_len);
                write(client_fd, message, n_len);*/
                if (image_response_message.head.len != 0) {
                    shareFree(pyramids_array[index]->image.data);
                }
                shareFree(pyramids_array[index]);

                /*
                struct image_response_protocol new_response_message;
                new_response_message.data = malloc(PIXEL_LENGTH(RECT_LENGTH(mesh_size)));
                net_to_image_response(message, sizeof(message), &new_response_message);*/
            }
        }
        shareFree(pyramids_array);
        shareFree(mesh_mark);
    }
}

static int flow_statistics = 0;

void client_transmission_proccess(int sockfd)
{
    int sever_fd = init_client_socket();
    //MeshHead *mesh_head;
    //Rect mesh_size;
    Rect mesh_num_size;
    struct program_protocol program_message_header;
    image_requst_protocol image_requst_message;
    image_response_protocol image_response_message;
    enum label_type label;
    int trans_len;

    char *message = malloc(MESSAGE_LEN);
    if (!message)
        return;

    while (true)
    {
        if ((trans_len = force_read(sockfd, (char*)&program_message_header, sizeof(program_message_header))) < 0)
            break;
        if(trans_len != sizeof(program_message_header) || program_message_header.protocol_label != REQUST_DECODE_IMAGE)
            continue;

        ImageVal *mesh_mark = getShareMemory(program_message_header.requst_decode.mesh_mark_key);
        ImagePyrDataType **pyramids_array = getShareMemory(program_message_header.requst_decode.pyramids_key);
        //mesh_head = getShareMemory(program_message_header.requst_decode.mesh_head_key);
        //mesh_size = program_message_header.requst_decode.mesh_size;
        mesh_num_size = program_message_header.requst_decode.mesh_num_size;

        image_requst_message.seq = program_message_header.requst_decode.seq;
        if (send_net_requst_image(sever_fd, &image_requst_message) < 0)
            break;

        /*label = REQUST_IMAGE;
        int n_len = 0;
        int n;
        if ((n = putLabelToNet(label, message, MESSAGE_LEN)) < 0)
            return;
        n_len += n;
        if ((n = image_requst_to_net(&image_requst_message, message + n_len, MESSAGE_LEN - n_len)) < 0)
            return;
        n_len += n;
        if (write(sever_fd, message, n_len) != n_len)
            break;*/

        //Mesh *mesh = mesh_head->mesh;
        int num_size = RECT_LENGTH(mesh_num_size);
        flow_statistics = 0;
        for (int i = 0; i < num_size; i++) {

            if ((label = recv_net_label(sever_fd)) < 0)
                return;

            flow_statistics += LABEL_SIZE;

            /*if ((trans_len = force_read(sever_fd, message, LABEL_SIZE)) != LABEL_SIZE) {
                return;
            }
            flow_statistics += trans_len;

            getLabelFromNet(message, trans_len, &label);*/
            if (label != RESPONSE_IMAGE) {
                i--;
                continue;
            }

            if ((trans_len = recv_net_response_image(sever_fd, &image_response_message) < 0))
                return;
            flow_statistics += trans_len;
/*            if ((trans_len = force_read(sever_fd, message, IMAGE_RESPONSE_HEAD_SIZE)) < 0) {
                return;
            }

            flow_statistics += trans_len;

            net_to_image_head_response(message, trans_len, &image_response_message);*/
            if (image_response_message.head.seq != program_message_header.requst_decode.seq) {
                i--;
                continue;
            }

            int index = image_response_message.head.point.y * mesh_num_size.width +
                        image_response_message.head.point.x;
            mesh_mark[index] = image_response_message.head.index_val;
            if(image_response_message.head.type == OLD) {
                pyramids_array[index] = NULL;
                continue;
            }

            pyramids_array[index] = shareMalloc(sizeof(ImagePyrDataType), ANONYMOUS_KEY);
            if (!pyramids_array[index])
                return;
            pyramids_array[index]->image.data = shareMalloc(image_response_message.head.len, ANONYMOUS_KEY);
            if (!pyramids_array[index]->image.data)
                return;
            pyramids_array[index]->node_layer = image_response_message.head.layer;
            if (force_read(sever_fd, (char*)pyramids_array[index]->image.data, image_response_message.head.len) < 0) {
                return;
            }

            flow_statistics += image_response_message.head.len;
            pyramids_array[index]->image.size = image_response_message.head.image_size;
            //仅复制PImage参数
            //memcpy(mesh[index].image.data, message, trans_len);
            //mesh[index].image.size = mesh_size;
        }
        program_message_header.protocol_label = RESPONSE_DECODE_IMAGE;
        program_message_header.response_decode.seq = image_requst_message.seq;
        program_message_header.response_decode.pyramids_key = getShareKey(pyramids_array);
        program_message_header.response_decode.mesh_mark_key = getShareKey(mesh_mark);
        printf("flow:%.2fKB\n", ((float )flow_statistics) / 1024);
        //getchar();
        if (write(sockfd, &program_message_header, sizeof(program_message_header)) != sizeof(program_message_header)) {
            break;
        }
    }
    free(message);
}