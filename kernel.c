#include <stdarg.h>
#include "memory-pi3.h"
#include "mailbox.h"
#include "font8x8.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

static uint32_t pt[8192] __attribute__((aligned(16)));
static int cursor_x = 0;
static int cursor_y = 0;

void init_screen();
void print_char(uint32_t *fb, int code);
void print_string(uint32_t *fb, char *str);

void kernel_main(unsigned int r0, unsigned int r1, unsigned int atags)
{
	init_screen();
	for (;;)
		;
}

void write_uint32(uint32_t *address, int n, ...)
{
	va_list valist;

	va_start(valist, n);
	for (int i = 0; i < n; i++)
	{
		*address++ = va_arg(valist, uint32_t);
	}
	va_end(valist);
}

void init_screen()
{
	#define FB_INDEX 24
	write_uint32(pt, 27,
					27 * 4, MAILBOX_REQUEST,													// 2
					MAILBOX_TAG_SET_PHYSICAL_WIDTH_HEIGHT, 8, 8, SCREEN_WIDTH, SCREEN_HEIGHT,	// 5
					MAILBOX_TAG_SET_VIRTUAL_WIDTH_HEIGHT, 8, 8, SCREEN_WIDTH, SCREEN_HEIGHT,	// 5
					MAILBOX_TAG_SET_COLOUR_DEPTH, 4, 4, 32,										// 4
					MAILBOX_TAG_SET_VIRTUAL_OFFSET, 8, 8, 0, 0,									// 5
					MAILBOX_TAG_ALLOCATE_FRAMEBUFFER, 8, 8, /* FB_INDEX */ 0, 0,				// 5
					0																			// 1
	);

	mailbox_write(MAILBOX_CHANNEL_TAGS, (uint32_t) pt);
	
	if((mailbox_read(MAILBOX_CHANNEL_TAGS) & ~0xF) == (uint32_t) pt)	// If first 28-bits and 0000 is pt
	{
		uint32_t *fb = (uint32_t *) BUSTOPHY(pt[FB_INDEX]);
		pt[FB_INDEX] = (uint32_t) fb;

		for (int x = 10; x < 20; x++)
		{
			*(fb + SCREEN_WIDTH * 100 + x) = 0xFF0000;
		}

		print_string(fb, "Hello,\nWorld! This is refactored!\n");
	}
	else
	{
		// There was error initialising framebuffer 
	}
}

void print_char(uint32_t *fb, int code)
{
	if (code == 10)
	{
		cursor_y += 9;
		cursor_x = 0;
		return;
	}

	char *char_map = font8x8_basic[code];
	uint32_t *fbp = fb + (cursor_x + cursor_y * SCREEN_WIDTH);

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
		fbp += SCREEN_WIDTH;
	}
	cursor_x += 9;
}

void print_string(uint32_t *fb, char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		print_char(fb, str[i]);
		i++;
	}
}
