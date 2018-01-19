TOOLCHAIN=arm-none-eabi
CC=$(TOOLCHAIN)-gcc
LD=$(TOOLCHAIN)-ld
OBJCOPY=$(TOOLCHAIN)-objcopy

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles

ifdef ComSpec
    RM=del /F /Q
	TONULL=>nul 2>&1
else
    RM=rm -f
	TONULL=> /dev/null 2>&1
endif

all: clean kernel7.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel7.img: start.o $(OBJS)
	$(LD) -nostdlib -nostartfiles start.o $(OBJS) -T pi3.ld -o kernel7.elf
	$(OBJCOPY) -O binary kernel7.elf kernel7.img

clean:
	$(RM) *.o *.elf *.img $(TONULL)
