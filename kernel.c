#include "font8x8.h"
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
static int pt[8192] __attribute__((aligned(16)));
static int cursor_x = 0;
static int cursor_y = 0;

void init_screen();
void print_char(unsigned char *fb, int code);
void print_string(unsigned char *fb, char *str);

void kernel_main(unsigned int r0, unsigned int r1, unsigned int atags)
{
	init_screen();

	for(;;);
}

void init_screen()
{
	int fb_index = -1;
	
	int i = 0;
	
	pt[i++] = -1; 				// Size in bytes. Populated at the end of this function.
	pt[i++] = 0x00000000;		// Request
	
	pt[i++] = 0x00048003;		// Set physical display
	pt[i++] = 8;
	pt[i++] = 8;
	pt[i++] = SCREEN_WIDTH;
	pt[i++] = SCREEN_HEIGHT;

	pt[i++] = 0x00048004;		// Set virtual buffer
	pt[i++] = 8;
	pt[i++] = 8;
	pt[i++] = SCREEN_WIDTH;
	pt[i++] = SCREEN_HEIGHT;
	
	pt[i++] = 0x00048005;		// Set depth
	pt[i++] = 4;
	pt[i++] = 4;
	pt[i++] = 8;
	
	pt[i++] = 0x00048009;		// Set virtual offset
	pt[i++] = 8;
	pt[i++] = 8;
	pt[i++] = 0;
	pt[i++] = 0;
	
	pt[i++] = 0x0004800B;		// Set palette
	pt[i++] = 0x00000010;
	pt[i++] = 0x00000010;
	pt[i++] = 0;
	pt[i++] = 2;
	pt[i++] = 0x00000000;
	pt[i++] = 0xFFFFFFFF;

	pt[i++] = 0x00040001;		// Allocate buffer
	pt[i++] = 0x00000008;
	pt[i++] = 0x00000008;
	fb_index = i;
	pt[i++] = 0;				// Framebuffer pointer
	pt[i++] = 0;

	pt[i++] = 0;				// End tag
	
	pt[0] = i * 4;
	
	do {
		unsigned int address = ((unsigned int) pt) + 8;
		unsigned int* mailbox_write = (unsigned int*) (0x3F000000 + 0xB880 + 0x20 + 0x8);
		*mailbox_write = address;
	} while(pt[fb_index] == 0);
	
	unsigned char* fb = (unsigned char*) (((unsigned int)pt[fb_index]) & 0x3FFFFFFF);
	pt[fb_index] = (unsigned int) fb;

	for(int x = 10; x < 20; x++)
	{
		*(fb + SCREEN_WIDTH * 10 + x) = 0x01;
	}	
	
	print_string(fb, "Hello,\nWorld!\n");
}

void print_char(unsigned char *fb, int code)
{
	if(code == 10)
	{
		cursor_y += 9;
		cursor_x = 0;
		return;
	}

	char *char_map = font8x8_basic[code];
	unsigned char *fbp = fb + (cursor_x + cursor_y * SCREEN_WIDTH);
	
	for(int r = 0; r < 8; r++)
	{
		char row_map = char_map[r];
		for(int b = 0; b < 8; b++)
		{
			if(row_map & 1 << b)
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
	while(str[i] != '\0')
	{
		print_char(fb, str[i]);
		i++;
	}
}