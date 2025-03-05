#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sensor_data.h"

// Simulated sensor data generation
void read_imu_data(float *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        data[i] = (float)rand() / (float)(RAND_MAX / 10.0); // Simulate IMU data
    }
}

void read_ppg_data(float *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        data[i] = (float)rand() / (float)(RAND_MAX / 5.0); // Simulate PPG data
    }
}

// Task to collect sensor data
void sensor_task(void *pvParameters) {
    QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;
    float imu_data[IMU_SAMPLE_RATE_HZ];
    float ppg_data[PPG_SAMPLE_RATE_HZ];

    while (1) {
        // Read sensor data
        read_imu_data(imu_data, IMU_SAMPLE_RATE_HZ);
        read_ppg_data(ppg_data, PPG_SAMPLE_RATE_HZ);

        // Pack data into a session structure
        session_t session;
        session.timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
        memcpy(session.imu_data, imu_data, sizeof(imu_data));
        memcpy(session.ppg_data, ppg_data, sizeof(ppg_data));

        // Send data to storage task via queue
        if (xQueueSend(sensor_data_queue, &session, portMAX_DELAY) != pdPASS) {
            printf("Sensor Task: Failed to send data to storage task. Retrying...\n");
            vTaskDelay(pdMS_TO_TICKS(100)); // Retry after a short delay
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(1000 / IMU_SAMPLE_RATE_HZ)); // Delay based on IMU sample rate
    }
}