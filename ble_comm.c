#include "ble_comm.h"
#include "flash_storage.h"
#include <sys/printk.h>
#include <zephyr/kernel.h>
#include <bluetooth/gatt.h>

// External variables for session control and thread management
extern bool session_active;
extern struct k_thread sensor_thread;
extern struct k_thread flash_data_thread;
extern struct k_msgq sensor_data_msgq; // Message queue for sensor data

// BLE Connection callback functions
static void handle_ble_connect(struct bt_conn *connection, uint8_t error) {
    if (error) {
        printk("BLE Connection failed (Error Code: %u)\n", error);
        return;
    }
    printk("BLE Device Connected Successfully\n");

    session_active = true;
}

// Called when Bluetooth device disconnects
static void handle_ble_disconnect(struct bt_conn *connection, uint8_t reason) {
    printk("BLE Disconnected (Reason Code: %u)\n", reason);

    if (session_active) {
        session_active = false;

        // Stop sensor data acquisition
        k_thread_suspend(&sensor_thread);
        printk("Sensor data collection suspended due to BLE disconnection\n");

        // Stop flash storage operations
        k_thread_suspend(&flash_data_thread);
        printk("Data storage halted due to BLE disconnection\n");
    }
}

// Structure to register BLE connection callbacks
static struct bt_conn_cb ble_conn_callbacks = {
    .connected = handle_ble_connect,
    .disconnected = handle_ble_disconnect,
};

// Function to initialize BLE communication
void ble_initialize(void) {
    int err = bt_enable(NULL);
    if (err) {
        printk("BLE Initialization failed (Error Code: %d)\n", err);
        return;
    }
    printk("BLE Stack Initialized\n");

    // Register BLE callbacks
    bt_conn_cb_register(&ble_conn_callbacks);
}

// Function to send sensor data over BLE
void ble_send_data(void) {
    if (!session_active) {
        printk("BLE session not active, cannot send data.\n");
        return;
    }

    struct sensor_session session;
    uint32_t flash_offset = FLASH_BASE_ADDRESS;

    // First, try sending live data from the queue
    while (k_msgq_get(&sensor_data_msgq, &session, K_NO_WAIT) == 0) {
        int err = bt_gatt_notify(NULL, DATA_TRANSFER_UUID, &session, sizeof(session));
        if (err) {
            printk("Failed to send live BLE data (Error Code: %d)\n", err);
        } else {
            printk("Live BLE Data sent successfully\n");
        }
    }

    // If no live data, try reading from flash
    while (flash_read_sensor_data(flash_offset, &session) == 0) {
        int err = bt_gatt_notify(NULL, DATA_TRANSFER_UUID, &session, sizeof(session));
        if (err) {
            printk("Failed to send stored BLE data (Error Code: %d)\n", err);
        } else {
            printk("Stored BLE Data sent successfully from flash offset 0x%08X\n", flash_offset);
        }
        flash_offset += sizeof(session);
    }
}
