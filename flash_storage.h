#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <zephyr.h>
#include <drivers/flash.h>

// Flash storage configuration
//These values are defined using Zephyr RTOSs' device tree macros
#define FLASH_BASE_ADDRESS DT_FLASH_AREA_STORAGE_OFFSET //starting address of flash memory
#define FLASH_AREA_SIZE DT_FLASH_AREA_STORAGE_SIZE //size of the flash

// Function declarations
void flash_init(void);
void flash_task(void);

#endif // FLASH_STORAGE_H
