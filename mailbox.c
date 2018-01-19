#include <stdint.h>
#include "memory-pi3.h"
#include "mailbox.h"

void mailbox_write(MailBoxChannel channel, uint32_t address)
{
	uint32_t addr_with_channel = address | channel;		                        // Add channel tag code to last 4 bits

	while(VAL_UINT32(MAILBOX1_STATUS) & MAILBOX_FULL) /* Do nothing */;			// Wait if mailbox is full
	VAL_UINT32(MAILBOX1_WRITE | channel) = addr_with_channel;
}

uint32_t mailbox_read(MailBoxChannel channel)
{
    for(;;)
	{
		while(VAL_UINT32(MAILBOX0_STATUS) & MAILBOX_EMPTY) /* Do nothing */;	// Wait if mailbox is empty 
		uint32_t r = VAL_UINT32(MAILBOX0_READ);									// Read message
		if((r & 0xF) == channel)					                            // If response is for provided channel
		{
			return r;
		}
	}
}