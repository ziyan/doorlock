#include <string.h>

#include "driver/flash.h"

#define FLASH_FWT 0x20

void flash_erase_page(const u8 *addr);
void flash_write_page_writer(void);
void flash_write_page(const u8 *addr, u8 *data);
void flash_read_page(const u8 *addr, u8 *data);

__no_init __xdata u16 flash_write_func[100];
volatile u8 *flash_write_data;


void flash_erase_page(const u8 *addr)
{
	__disable_interrupt();
	FADDRH = ((((u16)addr >> 1) & 0x3E00) >> 8);
	FADDRL = 0;
	FWT = FLASH_FWT;
	FCTL |= FCTL_ERASE;
	while (FCTL & FCTL_BUSY);
	FCTL &= ~FCTL_ERASE;
	__enable_interrupt();
}

void flash_write_page_writer(void)
{
	u16 i = 0;
	FCTL |= FCTL_WRITE;
	for (i = 0; i < FLASH_PAGE_SIZE;)
	{
		FWDATA = flash_write_data[i++];
		FWDATA = flash_write_data[i++];
		while(FCTL & FCTL_SWBSY);
	}
	FCTL &= ~FCTL_WRITE;
}


void flash_write_page(const u8 *addr, u8 *data)
{
	__disable_interrupt();
	FADDRH = ((((u16)addr >> 1) & 0x3E00) >> 8);
	FADDRL = 0;
	FWT = FLASH_FWT;
	flash_write_data = data;
	memcpy( (void*)flash_write_func, (void*)(u16)flash_write_page_writer, sizeof(flash_write_func));
	(*((VFPTR)(u16)flash_write_func))();
	while (FCTL & FCTL_BUSY);
	__enable_interrupt();
}

void flash_read_page(const u8 *addr, u8 *data)
{
	__disable_interrupt();
	FCTL |= FCTL_CONTRD;
	memcpy(data, addr, FLASH_PAGE_SIZE);
	FCTL &= ~FCTL_CONTRD;
	__enable_interrupt();
}

