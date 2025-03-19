#ifndef BLE_COMMUNICATION_H
#define BLE_COMMUNICATION_H

#include <zephyr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

// BLE UUID Definitions
#define BASE_UUID BT_UUID_DECLARE_128(0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, \
                                      0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21)
#define SESSION_START_UUID  BT_UUID_DECLARE_16(0x1234)
#define SESSION_STOP_UUID   BT_UUID_DECLARE_16(0x1235)
#define DATA_TRANSFER_UUID  BT_UUID_DECLARE_16(0x1237)

// Function declarations
void ble_initialize(void); // Initialize the Bluetooth stack
void ble_register_callbacks(void); // Register BLE callbacks
void ble_send_data(const void *data, size_t len); // Send data over BLE
void handle_ble_connect(struct bt_conn *connection, uint8_t error);
void handle_ble_disconnect(struct bt_conn *connection, uint8_t reason);

#endif // BLE_COMMUNICATION_H
