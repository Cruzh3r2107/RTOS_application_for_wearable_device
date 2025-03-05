#ifndef BLE_COMMUNICATION_H
#define BLE_COMMUNICATION_H

#include <zephyr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

// BLE UUID Definitions
#define BASE_UUID { 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21 }
#define SESSION_START_UUID  0x1234
#define SESSION_STOP_UUID   0x1235
#define DATA_TRANSFER_UUID  0x1237

// Function declarations
void ble_initialize(void);
void ble_register_callbacks(void);
void handle_ble_connect(struct bt_conn *connection, uint8_t error);
void handle_ble_disconnect(struct bt_conn *connection, uint8_t reason);

#endif // BLE_COMMUNICATION_H
