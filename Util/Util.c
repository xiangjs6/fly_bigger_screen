//
// Created by xjs on 2020/7/17.
//

#include "Util.h"

uint16_t checksum(uint16_t *data, size_t len)
{
    uint32_t sum = 0;
    while(len > 1){
        sum += *data++;
        len -= 2;
    }
    if(len == 1){
        sum += ((uint16_t) (*(uint8_t *) data) << 8);
    }
    while((sum >> 16) > 0)
        sum = (sum & 0xffff) + (sum >> 16);
    return (uint16_t)sum;
}