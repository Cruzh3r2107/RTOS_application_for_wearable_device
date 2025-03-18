#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <zephyr.h>
#include <drivers/i2c.h>

// Define IMU and PPG sampling rates
#define IMU_SAMPLING_RATE_HZ  50  // 50 Hz sampling rate
#define PPG_SAMPLING_RATE_HZ 100  // 100 Hz sampling rate

// Structure for sensor data storage
struct sensor_session {
    uint32_t timestamp;
    int16_t acc_x, acc_y, acc_z;  // Accelerometer data
    int16_t gyr_x, gyr_y, gyr_z;  // Gyroscope data
    uint32_t red, ir;             // PPG (Heart Rate) data
};

// Declare message queue for sensor data sharing
extern struct k_msgq sensor_data_msgq;

// Function declarations
void sensor_task(void);
int read_mpu6050_data(uint8_t *buffer);
int read_max86141_data(uint8_t *buffer);

// Declare I2C device
extern const struct device *i2c_dev;  // Required for I2C operations

#endif // SENSOR_DATA_H
