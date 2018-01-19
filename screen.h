#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdint.h>

typedef struct _ScreenInfo
{
    uint32_t    width;
    uint32_t    height;
    uint32_t    depth;
    void        *buffer;
} ScreenInfo;

void screen_init(ScreenInfo info);
void screen_putc(int code);
void screen_puts(char *str);

#endif