//
// Created by xjs on 2020/7/27.
//

#include "evdi.h"
#include "evdi_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>


static evdi_handle device_h;
static char *buff = NULL;
static ssize_t buff_size = 0;
static struct evdi_event_context content;
static bool buff_flag;

static void dpms_handler(int dpms_mode, void* user_data)
{
    printf("mdpms_mode:%d\n", dpms_mode);
    printf("dpms_handler\n");
}

static void mode_changed_handler(struct evdi_mode mode, void* user_data)
{
    //static int count = 0;
    printf("width:%d height:%d refresh:%d bits_per_pixel:%d pixel_format:%u\n", \
            mode.width, mode.height, mode.refresh_rate, mode.bits_per_pixel, mode.pixel_format);
    printf("mode_changed_handler\n");
    /*if(buff) {
        evdi_unregister_buffer(device_h, 0);
        free(buff);
    }
    buff = malloc(mode.height * mode.width * mode.bits_per_pixel / 8);
    buff_size = mode.height * mode.width * mode.bits_per_pixel / 8;
    memset(buff, 0, mode.height * mode.width * mode.bits_per_pixel / 8);
    struct evdi_buffer e_buff = {0, buff, mode.width, mode.height, \
                                    mode.width * mode.bits_per_pixel / 8};
    evdi_register_buffer(device_h, e_buff);*/
}

static void update_ready_handler(int buffer_to_be_updated, void* user_data)
{
    printf("update_ready_handler\n");
    buff_flag = true;
}

static void crtc_state_handler(int state, void* user_data)
{
    printf("crtc_state_handler");
}

static void cursor_set_handler(struct evdi_cursor_set cursor_set, void *user_data)
{
    printf("cursor_set_handler\n");
}

static void cursor_move_handler(struct evdi_cursor_move cursor_move, void *user_data)
{
    printf("cursor_move_handler\n");
}

int init_evdi(void *in_buff)
{
    buff = in_buff;
    unsigned char edid[1024];
    int device = 2;
    int fd = open("/home/xjs/target/bin/EDIDv2_1280x720", O_RDONLY);
    ssize_t edid_size = read(fd, edid, sizeof(edid));
    close(fd);

    content.update_ready_handler = update_ready_handler;
    content.crtc_state_handler = crtc_state_handler;
    content.cursor_move_handler = cursor_move_handler;
    content.cursor_set_handler = cursor_set_handler;
    content.dpms_handler = dpms_handler;
    content.mode_changed_handler = mode_changed_handler;
    content.user_data = device_h;

    if (evdi_check_device(device) != AVAILABLE) {
        device =  evdi_add_device();
        if (device < 0)
            return -1;
        printf("creat\n");
    }
    printf("available\n");
    device_h = evdi_open(device);
    evdi_connect(device_h, edid, edid_size, UINT_LEAST32_MAX);

    struct evdi_buffer e_buff = {0, buff, 1280, 720, \
                                    1280 * 4};
    evdi_register_buffer(device_h, e_buff);
    //evdi_enable_cursor_events(device_h);
    //evdi_handle_events(device_h, &content);

    return 0;
}

int get_screen(void)
{
    fd_set read_set;
    int n = 0;
    buff_flag = false;
    if (evdi_request_update(device_h, 0)) {
        struct evdi_rect rects[16];
        int rect_num;
        evdi_grab_pixels(device_h, rects, &rect_num);
        return 0;
    }

    FD_SET(evdi_get_event_ready(device_h), &read_set);
    struct timeval old_tv = {.tv_sec = 0, .tv_usec = 1000};
    struct timeval tv = old_tv;
    while (!buff_flag && (n = select(evdi_get_event_ready(device_h) + 1, &read_set, NULL, NULL, &tv)) >= 0) {
        if (n == 0)
            buff_flag = true;
        if(n > 0 && FD_ISSET(evdi_get_event_ready(device_h), &read_set)) {
            evdi_handle_events(device_h, &content);
        }
        printf("loop\n");
        FD_SET(evdi_get_event_ready(device_h), &read_set);
        tv = old_tv;
    }
    return 0;
}