#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "flash_storage.h"

// Simulate writing session data to flash
void write_session_to_flash(session_t *session) {
    printf("Flash Storage: Writing session %d to flash\n", session->session_id);
    // Implement actual flash write logic here
}

// Task to handle flash storage
void storage_task(void *pvParameters) {
    QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;
    SemaphoreHandle_t flash_mutex = (SemaphoreHandle_t)pvParameters;
    session_t session;

    while (1) {
        if (xQueueReceive(sensor_data_queue, &session, portMAX_DELAY) == pdPASS) {
            if (xSemaphoreTake(flash_mutex, portMAX_DELAY) == pdTRUE) {
                write_session_to_flash(&session); // Write session data to flash
                xSemaphoreGive(flash_mutex); // Release flash access
            } else {
                printf("Storage Task: Failed to acquire flash mutex\n");
            }
        } else {
            printf("Storage Task: Failed to receive data from sensor task\n");
        }
    }
}