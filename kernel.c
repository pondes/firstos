#include <stdarg.h>
#include "screen.h"

void kernel_main(unsigned int r0, unsigned int r1, unsigned int atags)
{
	ScreenInfo si;	
	si.width = 1920;
	si.height = 1080;
	si.depth = 32;
	screen_init(si);

	screen_puts("Hello, world!");

	for (;;);
}
