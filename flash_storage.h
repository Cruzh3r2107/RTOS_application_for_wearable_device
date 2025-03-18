#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <zephyr.h>
#include <drivers/flash.h>

// Flash storage configuration
#define FLASH_BASE_ADDRESS ((size_t)DT_FLASH_AREA_STORAGE_OFFSET) // Start address of flash memory
#define FLASH_AREA_SIZE ((size_t)DT_FLASH_AREA_STORAGE_SIZE) // Size of flash memory

// Function declarations
void flash_init(void);
void flash_task(void);
void flash_erase(void); // Function to erase flash memory

#endif // FLASH_STORAGE_H
