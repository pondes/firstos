#include <stdint.h>
#include "screen.h"
#include "memory-pi3.h"
#include "mailbox.h"
#include "font8x8.h"

static uint32_t pt[8192] __attribute__((aligned(16)));
static int cursor_x = 0;
static int cursor_y = 0;

static ScreenInfo screeninfo;

void screen_init(ScreenInfo info)
{
    screeninfo = info;

	#define FB_INDEX 24
	write_uint32(pt, 27,
					27 * 4, MAILBOX_REQUEST,													// 2
					MAILBOX_TAG_SET_PHYSICAL_WIDTH_HEIGHT, 8, 8, info.width, info.height,	    // 5
					MAILBOX_TAG_SET_VIRTUAL_WIDTH_HEIGHT, 8, 8, info.width, info.height,	    // 5
					MAILBOX_TAG_SET_COLOUR_DEPTH, 4, 4, info.depth,								// 4
					MAILBOX_TAG_SET_VIRTUAL_OFFSET, 8, 8, 0, 0,									// 5
					MAILBOX_TAG_ALLOCATE_FRAMEBUFFER, 8, 8, /* FB_INDEX */ 0, 0,				// 5
					0																			// 1
	);

	mailbox_write(MAILBOX_CHANNEL_TAGS, (uint32_t) pt);
	
	if((mailbox_read(MAILBOX_CHANNEL_TAGS) & ~0xF) == (uint32_t) pt)	// If first 28-bits and 0000 is pt
	{
		uint32_t *fb = (uint32_t *) BUSTOPHY(pt[FB_INDEX]);
		pt[FB_INDEX] = (uint32_t) fb;
        screeninfo.buffer = fb;
	}
	else
	{
		// There was error initialising framebuffer 
	}
}

void screen_putc(int code)
{
	if (code == 10)
	{
		cursor_y += 9;
		cursor_x = 0;
		return;
	}

	char *char_map = font8x8_basic[code];
	uint32_t *fbp = ((uint32_t *) screeninfo.buffer) + (cursor_x + cursor_y * screeninfo.width);

	for (int r = 0; r < 8; r++)
	{
		char row_map = char_map[r];
		for (int b = 0; b < 8; b++)
		{
			if (row_map & 1 << b)
			{
				fbp[b] = 0x00FF00;
			}
			else
			{
				fbp[b] = 0;
			}
		}
		fbp += screeninfo.width;
	}
	cursor_x += 9;
}

void screen_puts(char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		screen_putc(str[i]);
		i++;
	}
}
