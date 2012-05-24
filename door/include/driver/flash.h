#ifndef _DRIVER_FLASH_H_
#define _DRIVER_FLASH_H_

#include "project.h"

#define FLASH_PAGE_SIZE (1024u)
#define FLASH_PAGES (32u)

extern void flash_erase_page(const u8 *addr);
extern void flash_write_page(const u8 *addr, u8 *data);
extern void flash_read_page(const u8 *addr, u8 *data);

#endif /*_DRIVER_FLASH_H_*/

