#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "ble_communication.h"

// Simulate sending data over BLE
void send_data_over_ble(uint8_t *data, size_t len) {
    printf("BLE: Sending data over BLE: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// Simulate receiving a BLE command
void receive_ble_command(uint8_t *command) {
    *command = rand() % 2; // Simulate receiving a start (1) or stop (0) command
}

// Task to handle BLE communication
void ble_task(void *pvParameters) {
    QueueHandle_t ble_command_queue = (QueueHandle_t)pvParameters;
    uint8_t ble_command;

    while (1) {
        receive_ble_command(&ble_command); // Simulate receiving a BLE command

        if (xQueueSend(ble_command_queue, &ble_command, portMAX_DELAY) == pdPASS) {
            if (ble_command == 1) {
                printf("BLE Task: Start Session\n");
            } else if (ble_command == 0) {
                printf("BLE Task: Stop Session\n");
            }
        } else {
            printf("BLE Task: Failed to send command to queue\n");
        }
    }
}