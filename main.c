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
#include "Image/ImageHuffman.h"


int run = 1;
void sig_hup(int signo)
{
    run = 0;
}

void sig_term(int signo)
{
    destroyShareMemory();
    destoryDebug();
    printf("exit\n");
    exit(0);
}

int main(void)
{
    PImage image;
    image.data = malloc(PIXEL_LENGTH(RECT_LENGTH(((Rect){ 1280, 720}))));
    openImage(&image, "/home/xjs/screen_picture/15");
    HuffmanTree tree;
    char *out = malloc(PIXEL_LENGTH(RECT_LENGTH(((Rect){ 1280, 720}))));
    size_t len = PIXEL_LENGTH(RECT_LENGTH(((Rect){ 1280, 720})));
    size_t old_len = len;
    ImageHuffmanEncode(image, &tree, out, &len);
    printf("old_len:%d len:%d diff:%d rat:%f\n", old_len, len, old_len - len, (double)len / old_len);
    PImage image2;
    image2.data = malloc(PIXEL_LENGTH(RECT_LENGTH(((Rect){ 1280, 720}))));
    image2.size = (Rect){ 1280, 720};
    ImageHuffmanDecode(out, &tree, &image2);
    initDebug();
    showImage(image2.data, RECT_LENGTH(image2.size));
    int a = memcmp(image2.data, image.data, PIXEL_LENGTH(RECT_LENGTH(image.size)));
    printf("%d\n", a);
    getchar();
    return 0;
/*    HuffmanTree tree;
    HuffmanData data[] = {{.weight = 12, .value = 0}, {.weight = 4, .value = 1}, {.weight = 2, .value = 2}, {.weight = 10, .value = 3}, {.weight = 4, .value = 4}, {.weight = 9, .value = 5}, {.weight = 7, .value = 6}};
    //int32_t a[] = {1, 2, 2, 3, 3, 3, 3, 3, 6, 7, 6, 6, 1, 1, 11, 12, 11, 43};
    tree.tree = malloc(sizeof(HuffmanNode) * HUFFMAN_NODE_SIZE(sizeof(data) / sizeof(HuffmanData)));
    generateHuffmanCode(data, sizeof(data) / sizeof(HuffmanData), &tree, 0);
    for (int i = 0; i < tree.size; i++) {
        printf("pos:%d val:%d weight:%d l_child:%d r_child:%d\n", i, tree.tree[i].data.value, tree.tree[i].data.weight, tree.tree[i].children[0], tree.tree[i].children[1]);
    }
    HuffmanCode code[sizeof(data) / sizeof(HuffmanData)];
    for (int i = 0; i < sizeof(data) / sizeof(HuffmanData); i++)
        code[i].code = malloc(sizeof(data) / sizeof(HuffmanData));
    HuffmanTreeToCode(&tree, code, 0, NULL);
    return 0;*/
    int fd[2];
    int pid = 0;
    pid = fork();
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

    if (initShareMemory(1024 * 1024 * 10) < 0)
        return -1;
    if (pid == 0) {
        pid = fork();
        if (pid == 0) {
            printf("sever_transmission_proccess:%d\n", getpid());
            sever_transmission_proccess(fd[0]);
            printf("sever_transmission_proccess:awsl\n");
        } else {
            printf("image_encode_proccess:%d\n", getpid());
            image_encode_proccess(fd[1]);
            printf("image_encode_proccess:awsl\n");
        }
    } else {
        pid = fork();
        if (pid == 0) {
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
            printf("image_decode_proccess:awsl\n");
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
