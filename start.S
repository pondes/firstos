.section ".text"

.global _start

_start:
mrc 15,0,r0,cr0,cr0,#5
ands r0,#3
bne _loop
	
mov     sp, #(128 * (1024 * 1024)) - 4
b _cstartup

_loop:
b _loop
