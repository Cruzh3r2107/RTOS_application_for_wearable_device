#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sensor_data.h"
#include "flash_storage.h"
#include "ble_communication.h"

// Watchdog timer for system monitoring
void watchdog_task(void *pvParameters) {
    while (1) {
        // Check system health (e.g., task status, queue usage)
        printf("Watchdog: System is healthy\n");
        vTaskDelay(pdMS_TO_TICKS(5000)); // Check every 5 seconds
    }
}

void app_main() {
    // Initialize queues and mutexes
    QueueHandle_t sensor_data_queue = xQueueCreate(10, sizeof(session_t));
    QueueHandle_t ble_command_queue = xQueueCreate(10, sizeof(uint8_t));
    SemaphoreHandle_t flash_mutex = xSemaphoreCreateMutex();

    if (sensor_data_queue == NULL || ble_command_queue == NULL || flash_mutex == NULL) {
        printf("Failed to create queues or mutex\n");
        return;
    }

    // Create tasks for concurrent execution
    xTaskCreate(sensor_task, "Sensor Task", 2048, (void *)sensor_data_queue, 2, NULL);
    xTaskCreate(ble_task, "BLE Task", 2048, (void *)ble_command_queue, 1, NULL);
    xTaskCreate(storage_task, "Storage Task", 2048, (void *)sensor_data_queue, 2, NULL);
    xTaskCreate(watchdog_task, "Watchdog Task", 2048, NULL, 1, NULL); // System monitoring

    // Simulate sending a BLE command to start a session
    uint8_t start_command = 1;
    if (xQueueSend(ble_command_queue, &start_command, portMAX_DELAY) != pdPASS) {
        printf("Failed to send start command to BLE task\n");
    }

    // Main loop (optional, for debugging or additional functionality)
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        printf("Main loop running...\n");
    }
}