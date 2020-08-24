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

char *strrev(char *s)
{
    /* h指向s的头部 */
    char *h = s;
    char *t = s;
    char ch;

    /* t指向s的尾部 */
    while(*t++){};
    t--;    /* 与t++抵消 */
    t--;    /* 回跳过结束符'\0' */

    /* 当h和t未重合时，交换它们所指向的字符 */
    while(h < t)
    {
        ch = *h;
        *h++ = *t;    /* h向尾部移动 */
        *t-- = ch;    /* t向头部移动 */
    }

    return(s);
}