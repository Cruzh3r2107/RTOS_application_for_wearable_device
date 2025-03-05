#include <zephyr.h>
#include <drivers/i2c.h> //Provides I2C driver APIs for sensor communication
#include <sys/printk.h>
#include "sensor_data.h"
#include "flash.h"
#include "ble_comm.h"

// Define task priorities and stack sizes
#define STACK_SIZE 1024 // stack size for each thread
#define PRIORITY_BLE_TASK 3 //lowest priority
#define PRIORITY_FLASH_TASK 2
#define PRIORITY_SENSOR_TASK 1 //highest priority 

// Message queue for sensor data
K_MSGQ_DEFINE(sensor_data_msgq, sizeof(struct sensor_session), 10, 4);

// Thread control blocks for the sensor, flash, and BLE tasks
static struct k_thread sensor_data_thread;
static struct k_thread flash_data_thread;
static struct k_thread ble_thread;

// Thread IDs
k_tid_t sensor_thread_id = NULL;
k_tid_t flash_thread_id = NULL;
static k_tid_t ble_thread_id = NULL;

// Stack  definition for each thread using a macro
K_THREAD_STACK_DEFINE(sensor_stack_area, STACK_SIZE);
K_THREAD_STACK_DEFINE(flash_stack_area, STACK_SIZE);
K_THREAD_STACK_DEFINE(ble_stack_area, STACK_SIZE);

bool is_session_active = false;

// Main function
void main(void) 
{
    printk("System Initializing...\n"); //log a message indicating the start of the system

    // Initialize Flash Storage
    flash_init();

    // Initialize BLE Services
    int ble_status = ble_init();
    if (ble_status != 0) 
    {
        printk("Bluetooth initialization failed!\n");
        return;
    }

    // Initialize Sensor I2C communication (if needed)
    i2c_dev = i2c_init();
    if (!i2c_dev) {
        printk("I2C initialization failed!\n");
        return;
    }

    // Start BLE thread
    ble_thread_id = k_thread_create(&ble_thread, ble_stack_area, 
                                    K_THREAD_STACK_SIZEOF(ble_stack_area),
                                    ble_init, NULL, NULL, NULL, 
                                    PRIORITY_BLE_TASK, 0, K_NO_WAIT); // The ble thread starts the ble_init func
    printk("BLE thread started\n");

    // Start Sensor Data Acquisition thread
    sensor_thread_id = k_thread_create(&sensor_data_thread, sensor_stack_area, 
                                       K_THREAD_STACK_SIZEOF(sensor_stack_area),
                                       sensor_task, NULL, NULL, NULL, 
                                       PRIORITY_SENSOR_TASK, 0, K_NO_WAIT);
    printk("Sensor data thread started\n");

    // Start Flash Storage thread
    flash_thread_id = k_thread_create(&flash_data_thread, flash_stack_area, 
                                      K_THREAD_STACK_SIZEOF(flash_stack_area),
                                      flash_task, NULL, NULL, NULL, 
                                      PRIORITY_FLASH_TASK, 0, K_NO_WAIT);
    printk("Flash storage thread started\n");

    printk("System Initialization Complete!\n");
}
/*
The kernel ensures that tasks will execute based on priorty like I have defined 
above.
Meta data stored = TCB
Thread stack is defined above 
Thread ID is assigned by the kernel when the threads are created.
Priority-based preemptive scheduler
----------------------------------------
-->Runs frequently due to its high priority.
-->Preempts the flash and BLE tasks when it is ready.
-->Runs when the sensor task is blocked (e.g., waiting for I2C data).
-->Preempts the BLE task but is preempted by the sensor task.
-->Runs when both the sensor and flash tasks are blocked.
-->Can be preempted by the sensor or flash tasks.

*/