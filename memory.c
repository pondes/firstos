#include <stdint.h>
#include <stdarg.h>
#include "memory-pi3.h"

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
