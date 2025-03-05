#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <zephyr.h>
#include <drivers/flash.h>

// Flash storage configuration
#define FLASH_BASE_ADDRESS DT_FLASH_AREA_STORAGE_OFFSET
#define FLASH_AREA_SIZE DT_FLASH_AREA_STORAGE_SIZE

// Structure for sensor session data
struct sensor_session {
    uint32_t timestamp;
    int16_t acc_x, acc_y, acc_z;
    int16_t gyr_x, gyr_y, gyr_z;
    uint32_t red, ir;
};

// Function declarations
void flash_init(void);
void flash_task(void);

#endif // FLASH_STORAGE_H
