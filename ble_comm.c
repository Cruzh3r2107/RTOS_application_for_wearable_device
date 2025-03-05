#include "ble_comm.h"
#include <sys/printk.h>

// External variables for session control and thread management
extern bool session_active;
extern k_tid_t sensor_task_id;
extern k_tid_t storage_task_id;

// Connection callback functions
static void handle_ble_connect(struct bt_conn *connection, uint8_t error) {
    if (error) {
        printk("BLE Connection failed (Error Code: %u)\n", error);
        return;
    }
    printk("BLE Device Connected Successfully\n");
}

static void handle_ble_disconnect(struct bt_conn *connection, uint8_t reason) {
    printk("BLE Disconnected (Reason Code: %u)\n", reason);

    if (session_active) {
        session_active = false;

        // Stop sensor data acquisition
        if (sensor_task_id) {
            k_thread_abort(sensor_task_id);
            sensor_task_id = NULL;
            printk("Sensor data collection stopped due to BLE disconnection\n");
        }

        // Stop storage operations
        if (storage_task_id) {
            k_thread_abort(storage_task_id);
            storage_task_id = NULL;
            printk("Data storage halted due to BLE disconnection\n");
        }
    }
}

// Register BLE connection callbacks
static struct bt_conn_cb ble_conn_callbacks = {
    .connected = handle_ble_connect,
    .disconnected = handle_ble_disconnect,
};

// Function to initialize BLE communication
void ble_initialize(void) {
    int err = bt_enable(NULL);
    if (err) {
        printk("BLE Initialization failed (Error Code: %d)\n", err);
    } else {
        printk("BLE Stack Initialized\n");
    }
}

// Function to register connection callbacks
void ble_register_callbacks(void) {
    bt_conn_cb_register(&ble_conn_callbacks);
}
