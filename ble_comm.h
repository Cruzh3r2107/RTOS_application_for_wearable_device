#ifndef BLE_COMMUNICATION_H
#define BLE_COMMUNICATION_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Function prototypes for BLE communication
void ble_task(void *pvParameters); // Main BLE task
void send_data_over_ble(uint8_t *data, size_t len); // Send data over BLE
void receive_ble_command(uint8_t *command); // Receive a command from BLE

#endif // BLE_COMMUNICATION_H