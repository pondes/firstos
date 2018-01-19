#ifndef _MEMORY_PI3_H_
#define _MEMORY_PI3_H_

#include <stdint.h>

#define IO_BASE                 0x3F000000

#define VAL_UINT32(addr)        *((volatile uint32_t *) (addr))
#define BUSTOPHY(addr)          (addr & 0x3FFFFFFF)

void write_uint32(uint32_t *address, int n, ...);

#endif