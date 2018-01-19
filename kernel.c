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
void print_char(unsigned char *fb, int code);
void print_string(unsigned char *fb, char *str);

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
	#define FB_INDEX 31
	write_uint32(pt, 34,
					34 * 4, MAILBOX_REQUEST,													// 2
					MAILBOX_TAG_SET_PHYSICAL_WIDTH_HEIGHT, 8, 8, SCREEN_WIDTH, SCREEN_HEIGHT,	// 5
					MAILBOX_TAG_SET_VIRTUAL_WIDTH_HEIGHT, 8, 8, SCREEN_WIDTH, SCREEN_HEIGHT,	// 5
					MAILBOX_TAG_SET_COLOUR_DEPTH, 4, 4, 8,										// 4
					MAILBOX_TAG_SET_VIRTUAL_OFFSET, 8, 8, 0, 0,									// 5
					MAILBOX_TAG_SET_PALETTE, 0x10, 0x10, 0, 2, 0x00000000, 0xFFFFFFFF,			// 7
					MAILBOX_TAG_ALLOCATE_FRAMEBUFFER, 8, 8, /* FB_INDEX */ 0, 0,				// 5
					0																			// 1
	);

	uint32_t addr_with_channel = ((uint32_t) pt) | MAILBOX_CHANNEL_TAGS;		// Add channel tag code to last 4 bits

	while(VAL_UINT32(MAILBOX1_STATUS) & MAILBOX_FULL) /* Do nothing */;			// Wait if mailbox is full
	VAL_UINT32(MAILBOX1_WRITE + MAILBOX_CHANNEL_TAGS) = addr_with_channel;		// Write address of message
	
	for(;;)
	{
		while(VAL_UINT32(MAILBOX0_STATUS) & MAILBOX_EMPTY) /* Do nothing */;	// Wait if mailbox is empty 
		uint32_t r = VAL_UINT32(MAILBOX0_READ);									// Read message
		if(r == addr_with_channel)					// If it is a response and it is for our request
		{
			break;			
		}
	}

	unsigned char *fb = (unsigned char *) BUSTOPHY(pt[FB_INDEX]);
	pt[FB_INDEX] = (uint32_t) fb;

	for (int x = 10; x < 20; x++)
	{
		*(fb + SCREEN_WIDTH * 10 + x) = 0x01;
	}

	print_string(fb, "Hello,\nWorld! This is refactored!\n");
}

void print_char(unsigned char *fb, int code)
{
	if (code == 10)
	{
		cursor_y += 9;
		cursor_x = 0;
		return;
	}

	char *char_map = font8x8_basic[code];
	unsigned char *fbp = fb + (cursor_x + cursor_y * SCREEN_WIDTH);

	for (int r = 0; r < 8; r++)
	{
		char row_map = char_map[r];
		for (int b = 0; b < 8; b++)
		{
			if (row_map & 1 << b)
			{
				fbp[b] = 1;
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

void print_string(unsigned char *fb, char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		print_char(fb, str[i]);
		i++;
	}
}
