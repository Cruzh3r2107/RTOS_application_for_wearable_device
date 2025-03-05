#include "flash.h"
#include <drivers/flash.h>
#include <sys/printk.h>
#include <string.h>

// Mutex for flash memory access
K_MUTEX_DEFINE(flash_mutex); //for thread safe access

// Message queue for sensor data
extern K_MSGQ_DEFINE(sensor_data_msgq, sizeof(struct sensor_session), 10, 4);

// Flash device pointer
static const struct device *flash_dev;

// Flash task to store data
void flash_task(void) {
    struct sensor_session session;
    size_t offset = FLASH_BASE_ADDRESS;

    while (1) 
    {
        if (k_msgq_get(&sensor_data_msgq, &session, K_FOREVER) == 0) {
            k_mutex_lock(&flash_mutex, K_FOREVER);
            //checks if there is enough memory in the flash
            if (offset + sizeof(session) <= FLASH_AREA_SIZE)
            {
                // Flash_write is a API defined in drivers/flash.h
                if (flash_write(flash_dev, offset, &session, sizeof(session)) != 0) 
                {
                    printk("Flash write failed\n");
                }
                offset += sizeof(session);
            } 
            else 
            {
                printk("Flash memory full. Stopping session.\n");
            }

            k_mutex_unlock(&flash_mutex);
        }
    }
}

// Initialize Flash device
void flash_init(void) 
{
    //Retrieves the flash device
    flash_dev = device_get_binding(DT_LABEL(DT_NODELABEL(flash0)));

    if (!flash_dev) 
    {
        printk("No Flash device found\n");
    } 
    else 
    {
        printk("Flash initialized\n");
    }
}
