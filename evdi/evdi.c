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
    //unsigned char edid[1024];
    //1600*900
    u_int8_t edid[128] = {
            0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0xaf, 0x3e, 0x21, 0x00, 0x00, 0x00, 0x00,
            0x21, 0x14, 0x01, 0x04, 0xa0, 0x1f, 0x11, 0x78, 0x02, 0x61, 0x95, 0x9c, 0x59, 0x52, 0x8f, 0x26,
            0x21, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
            0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xf8, 0x2a, 0x40, 0x9a, 0x61, 0x84, 0x0c, 0x30, 0x40, 0x2a,
            0x33, 0x00, 0x35, 0xae, 0x10, 0x00, 0x00, 0x18, 0xa5, 0x1c, 0x40, 0x9a, 0x61, 0x84, 0x0c, 0x30,
            0x40, 0x2a, 0x33, 0x00, 0x35, 0xae, 0x10, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x41,
            0x55, 0x4f, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfe,
            0x00, 0x42, 0x31, 0x34, 0x30, 0x52, 0x57, 0x30, 0x32, 0x20, 0x56, 0x31, 0x20, 0x0a, 0x00, 0xc0,
    };
    size_t edid_size = sizeof(edid);
    int device = -1;
    /*int fd = open("/home/xjs/target/bin/EDIDv1_1600x900", O_RDONLY);
    size_t edid_size = read(fd, edid, sizeof(edid));
    close(fd);*/

    content.update_ready_handler = update_ready_handler;
    content.crtc_state_handler = crtc_state_handler;
    content.cursor_move_handler = cursor_move_handler;
    content.cursor_set_handler = cursor_set_handler;
    content.dpms_handler = dpms_handler;
    content.mode_changed_handler = mode_changed_handler;
    content.user_data = device_h;

    for (int i = 0; i < 100; i++)
        if (evdi_check_device(i) == AVAILABLE) {
            device = i;
            break;
        }

    printf("devides:%d\n", device);
    if (device == -1) {
        device =  evdi_add_device();
        if (device < 0)
            return -1;
        printf("creat\n");
    }
    device_h = evdi_open(device);
    evdi_connect(device_h, edid, edid_size, UINT_LEAST32_MAX);

    struct evdi_buffer e_buff = {0, buff, 1600, 900, \
                                    1600 * 4};
    evdi_register_buffer(device_h, e_buff);
    //evdi_enable_cursor_events(device_h);
    //evdi_handle_events(device_h, &content);

    return 0;
}

int get_screen(long int us)
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
    struct timeval old_tv = {.tv_sec = 0, .tv_usec = us};
    struct timeval tv = old_tv;
    struct timeval *p_tv = us == -1 ? NULL : &tv;
    while (!buff_flag && (n = select(evdi_get_event_ready(device_h) + 1, &read_set, NULL, NULL, p_tv)) >= 0) {
        if (n == 0)
            buff_flag = true;
        if(n > 0 && FD_ISSET(evdi_get_event_ready(device_h), &read_set)) {
            evdi_handle_events(device_h, &content);
        }
        FD_SET(evdi_get_event_ready(device_h), &read_set);
        tv = old_tv;
    }
    return 0;
}