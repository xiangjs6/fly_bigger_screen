//
// Created by xjs on 2020/8/3.
//

#include "debug.h"
#include <stdio.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include <fcntl.h>

static SDL_Surface* screen = NULL;

int initDebug(void)
{
    if (SDL_Init( SDL_INIT_EVERYTHING ) < 0)
        return -1;
    screen = SDL_SetVideoMode( 1600, 900, 32, SDL_SWSURFACE );
    if(screen == NULL)
        return -1;
    return 0;
}

void destoryDebug(void)
{
    SDL_FreeSurface(screen);
    SDL_Quit();
}

void showImage(void *ptr, size_t rect_size)
{
    BGR2BGRA(ptr, screen->pixels, rect_size);
    //memcpy(screen->pixels, ptr, PIXEL_LENGTHBGRA(rect_size));
    SDL_Flip( screen );
}

void writetofile(PImage image)
{
    static int index = 0;
    char path[1024];
    sprintf(path, "/home/xjs/screen_picture/%d.out", index++);
    int fd = open(path, O_CREAT | O_RDWR | O_TRUNC);
    write(fd, image.data, PIXEL_LENGTH(RECT_LENGTH(image.size)));
    close(fd);
}

void openImage(PImage *image, char *path)
{
    Rect size = {.width = 1600, .height = 900};
    image->size = size;
    void *buff = malloc(PIXEL_LENGTHBGRA(RECT_LENGTH(size)));
    int fd = open(path, O_RDWR);
    read(fd, buff, PIXEL_LENGTHBGRA(RECT_LENGTH(size)));
    BGRA2BGR(buff, image->data, RECT_LENGTH(size));
    close(fd);
}
