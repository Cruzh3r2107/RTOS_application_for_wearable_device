#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <zephyr.h>
#include <drivers/i2c.h>

// Define structure for sensor data storage
struct sensor_session {
    uint32_t timestamp;
    int16_t acc_x, acc_y, acc_z;  // Accelerometer data
    int16_t gyr_x, gyr_y, gyr_z;  // Gyroscope data
    uint32_t red, ir;             // PPG (Heart Rate) data
};

// Message queue for sensor data sharing
extern K_MSGQ_DECLARE(sensor_data_msgq);

// Function declarations
void sensor_task(void);
int read_mpu6050_data(uint8_t *buffer);
int read_max86141_data(uint8_t *buffer);

#endif
