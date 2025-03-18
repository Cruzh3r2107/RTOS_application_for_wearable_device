#include "flash_storage.h"
#include <drivers/flash.h>
#include <sys/printk.h>
#include <string.h>

// Mutex for flash memory access
K_MUTEX_DEFINE(flash_mutex); // Ensures thread-safe access

// Message queue for sensor data
extern struct k_msgq sensor_data_msgq; // Correct extern declaration

// Flash device pointer
static const struct device *flash_dev;

// Function to erase flash memory
void flash_erase(void) {
    k_mutex_lock(&flash_mutex, K_FOREVER);

    printk("Erasing flash memory...\n");
    if (flash_erase(flash_dev, FLASH_BASE_ADDRESS, FLASH_AREA_SIZE) != 0) {
        printk("Flash erase failed!\n");
    } else {
        printk("Flash memory erased successfully.\n");
    }

    k_mutex_unlock(&flash_mutex);
}

// Flash task to store data
void flash_task(void) {
    struct sensor_session session;
    size_t offset = FLASH_BASE_ADDRESS;

    while (1) {
        if (k_msgq_get(&sensor_data_msgq, &session, K_FOREVER) == 0) {
            k_mutex_lock(&flash_mutex, K_FOREVER);

            // Check if there's enough space in flash
            if (offset + sizeof(session) <= FLASH_BASE_ADDRESS + FLASH_AREA_SIZE) {
                // Disable write protection before writing
                flash_write_protection_set(flash_dev, false);

                // Write sensor data to flash
                if (flash_write(flash_dev, offset, &session, sizeof(session)) != 0) {
                    printk("Flash write failed at offset 0x%08X\n", offset);
                } else {
                    printk("Data written to flash at offset 0x%08X\n", offset);
                    offset += sizeof(session);
                }

                // Re-enable write protection
                flash_write_protection_set(flash_dev, true);
            } else {
                printk("Flash memory full. Erasing and restarting...\n");
                flash_erase();
                offset = FLASH_BASE_ADDRESS; // Reset offset
            }

            k_mutex_unlock(&flash_mutex);
        }
    }
}

// Initialize Flash device
void flash_init(void) {
    // Retrieve the flash device
    flash_dev = device_get_binding(DT_LABEL(DT_NODELABEL(flash0)));

    if (!flash_dev) {
        printk("No Flash device found\n");
    } else {
        printk("Flash initialized\n");
    }
}
