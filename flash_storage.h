#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sensor_data.h"

// Function prototypes for flash storage interaction
void storage_task(void *pvParameters); // Main storage task
void write_session_to_flash(session_t *session); // Write session data to flash
void read_session_from_flash(uint32_t session_id, session_t *session); // Read session data from flash

#endif // FLASH_STORAGE_H