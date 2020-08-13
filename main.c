#include "evdi/evdi_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include "Image/PImage.h"
#include "Util/Util.h"
#include "Image/ImageHash.h"
#include "Image/ImageMesh.h"
#include "Image/ImageCode.h"
#include "protocols/program_protocol.h"
#include "debug.h"
#include "protocols/transmission.h"
#include <signal.h>
#include "Image/ImagePyrTree.h"


int run = 1;
void sig_hup(int signo)
{
    run = 0;
}

int init_socket(void)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock_fd, &addr, sizeof(addr));
    listen(sock_fd, 2);
    int fd = accept(sock_fd, NULL, NULL);
    close(sock_fd);
    return fd;
}

void sig_term(int signo)
{
    destroyShareMemory();
    destoryDebug();
    printf("exit\n");
    exit(0);
}

#include "protocols/protocols.h"
int main(void)
{
    initDebug();
    PImage src_image;
    PImage image;
    image.size = (Rect){.width = 1280, .height = 720};
    src_image.size = image.size;
    image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(image.size)));
    src_image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(image.size)));
    openImage(&image, "/home/xjs/screen_picture/1");
    imageCopy(src_image, image, ORIGIN_POINT, ORIGIN_POINT, image.size);
    ImagePyrTree tree = initImagePyrTree(2);
    ImagePyrTree merge = initImagePyrTree(2);
    ImagePyrDataType *p;
    ImagePyramid out;
    //splitPyramid(image, &out);

    imagePyramid(&tree, image);
    ImagePyrDataType pyramid;
    pyramid.image.size = (Rect){.width = 320, .height = 180};
    pyramid.image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(pyramid.image.size)));
    StackDataType data = {.p_val = &pyramid};
    for (int i = 0; i < 16; i++) {
        popStack(&tree.stack, &data);
        putPyramid(&merge, pyramid);
    }

    p = merge.max_size_pyramid;
    int a = memcmp(src_image.data, p->image.data, PIXEL_LENGTH(RECT_LENGTH(src_image.size)));
    printf("%d\n", a);
    showImage(p->image.data, RECT_LENGTH(p->image.size));
    getchar();
    destoryDebug();
    return 0;
/*    Pixel data[] = {1, 1, 1, 1, 0, 0, 0, 0, 4, 4, 4, 4,\
                    2, 2, 2, 2, 3, 3, 3, 3, 5, 5, 5, 5};
    PImage image;
    image.size = (Rect){3, 2};
    image.data = data;
    PImage d_image;
    d_image.size = (Rect){2, 2};
    Pixel d_data[4];
    d_image.data = d_data;
    imageCopy(d_image, image, (Point){0, 0}, (Point){0, 0}, d_image.size);
    return 0;*/
    int fd[2];
    //int pid = fork();
    int pid = 1;
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

    if (initShareMemory(1024000000) < 0)
        return -1;
    if (pid != 0) {
        pid = fork();
        if (pid != 0) {
            printf("sever_transmission_proccess:%d\n", getpid());
            sever_transmission_proccess(fd[0]);
            printf("sever_transmission_proccess:awsl\n");
        } else {
            printf("image_encode_proccess:%d\n", getpid());
            image_encode_proccess(fd[1]);
        }
    } else {
        pid = fork();
        if (pid != 0) {
            printf("client_transmission_proccess:%d\n", getpid());
            sleep(2);
            client_transmission_proccess(fd[0]);
            printf("client_transmission_proccess:awsl\n");
        } else {
            signal(SIGTERM, sig_term);
            if (initDebug() < 0)
                return -1;
            printf("image_decode_proccess:%d\n", getpid());
            image_decode_proccess(fd[1]);
            destoryDebug();
        }
    }
    destroyShareMemory();
    return 0;
}

/*
int main(void)
{
    //printf("%d\n", evdi_add_device());
    //return 0;
    int client_fd = init_socket();
    printf("%d\n", client_fd);
    signal(SIGHUP, sig_hup);
    int device = 2;
    unsigned char edid[1024];
    int fd = open("/home/xjs/target/bin/EDIDv2_1280x720", O_RDONLY);
    ssize_t edid_size = read(fd, edid, sizeof(edid));
    close(fd);

    struct evdi_event_context content;
    content.update_ready_handler = update_ready_handler;
    content.crtc_state_handler = crtc_state_handler;
    content.cursor_move_handler = cursor_move_handler;
    content.cursor_set_handler = cursor_set_handler;
    content.dpms_handler = dpms_handler;
    content.mode_changed_handler = mode_changed_handler;
    content.user_data = device_h;

    if(evdi_check_device(device) == AVAILABLE)
    {
        int index = 0;
        int count = 0;
        printf("available\n");
        device_h = evdi_open(device);
        evdi_connect(device_h, edid, edid_size, UINT_LEAST32_MAX);
        //evdi_enable_cursor_events(device_h);
        //evdi_handle_events(device_h, &content);
        fd_set read_set;
        while(run)
        {
            FD_SET(evdi_get_event_ready(device_h), &read_set);
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 10000;
            int n = select(evdi_get_event_ready(device_h) + 1, &read_set, NULL, NULL, &tv);
            if(n > 0 && FD_ISSET(evdi_get_event_ready(device_h), &read_set)) {
                evdi_handle_events(device_h, &content);
            }
            if(buff && evdi_request_update(device_h, 0)){
                struct evdi_rect rects[16];
                int rect_num;
                evdi_grab_pixels(device_h, rects, &rect_num);
                for (int i = 0; i < rect_num; ++i) {
                    printf("[%d,%d,%d,%d]\n", rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
                }
                if(rect_num > 1)
                    return 0;
                printf("buff[100]:%d\n", buff[100]);
                write(client_fd, buff, buff_size);
                if(count++ % 300 == 0) {
                    char path[1024];
                    sprintf(path, "/home/xjs/screen_picture/%d", index++);
                    int fd = open(path, O_CREAT | O_RDWR | O_TRUNC);
                    write(fd, buff, buff_size);
                    close(fd);
                }
            }
        }
        evdi_disconnect(device_h);
        evdi_close(device_h);
        return 1;
    }
    return 0;
}
*/
