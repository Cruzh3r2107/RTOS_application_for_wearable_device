#include "sensor_data.h"
#include <sys/printk.h>
#include <drivers/i2c.h>

// Define sensor I2C addresses
#define MPU6050_I2C_ADDR  0x68  // IMU sensor address
#define MAX86141_I2C_ADDR 0x55  // PPG sensor address

// Define message queue for sensor data
K_MSGQ_DEFINE(sensor_data_msgq, sizeof(struct sensor_session), 10, 4); // Queue size: 10 messages

// I2C device instance
const struct device *i2c_dev;

// Sensor task: Reads sensor data and pushes to message queue
void sensor_task(void) {
    uint8_t imu_buffer[14]; // MPU6050 returns 14 bytes (Accel + Gyro + Temp)
    uint8_t ppg_buffer[6];  // MAX86141 returns 6 bytes

    // Get I2C device binding
    i2c_dev = device_get_binding("I2C_1");
    if (!i2c_dev) {
        printk("I2C device not found!\n");
        return;
    }

    while (1) {
        struct sensor_session session;

        // Read IMU data (50Hz)
        session.timestamp = k_uptime_get_32(); // Get system timestamp
        if (read_mpu6050_data(imu_buffer) != 0) {
            printk("IMU read failed. Stopping session.\n");
            break;
        }

        // Parse IMU Data (MPU6050)
        session.acc_x = (imu_buffer[0] << 8) | imu_buffer[1];
        session.acc_y = (imu_buffer[2] << 8) | imu_buffer[3];
        session.acc_z = (imu_buffer[4] << 8) | imu_buffer[5];
        session.gyr_x = (imu_buffer[8] << 8) | imu_buffer[9];
        session.gyr_y = (imu_buffer[10] << 8) | imu_buffer[11];
        session.gyr_z = (imu_buffer[12] << 8) | imu_buffer[13];

        // Attempt to send IMU data to message queue
        if (k_msgq_put(&sensor_data_msgq, &session, K_NO_WAIT) != 0) {
            printk("Sensor queue full. Dropping IMU data.\n");
        }

        // Read PPG data (100Hz)
        for (int i = 0; i < 2; i++) {  // Since PPG is sampled at 100Hz (2x faster)
            session.timestamp = k_uptime_get_32(); // Update timestamp for PPG
            if (read_max86141_data(ppg_buffer) != 0) {
                printk("PPG read failed. Stopping session.\n");
                break;
            }

            // Parse PPG Data (MAX86141)
            session.red = (ppg_buffer[0] << 16) | (ppg_buffer[1] << 8) | ppg_buffer[2];
            session.ir  = (ppg_buffer[3] << 16) | (ppg_buffer[4] << 8) | ppg_buffer[5];

            // Attempt to send PPG data to message queue
            if (k_msgq_put(&sensor_data_msgq, &session, K_NO_WAIT) != 0) {
                printk("Sensor queue full. Dropping PPG data.\n");
            }

            // Sleep to achieve 100Hz sampling rate
            k_sleep(K_MSEC(1000 / PPG_SAMPLING_RATE_HZ));
        }

        // Sleep to achieve 50Hz sampling rate for IMU
        k_sleep(K_MSEC(1000 / IMU_SAMPLING_RATE_HZ));
    }
}

// Read IMU data via I2C (MPU6050)
int read_mpu6050_data(uint8_t *buffer) {
    int ret = i2c_write_read(i2c_dev, MPU6050_I2C_ADDR, (uint8_t[]){0x3B}, 1, buffer, 14);
    if (ret != 0) {
        printk("Error reading MPU6050 data!\n");
    }
    return ret;
}

// Read PPG data via I2C (MAX86141)
int read_max86141_data(uint8_t *buffer) {
    int ret = i2c_write_read(i2c_dev, MAX86141_I2C_ADDR, (uint8_t[]){0x07}, 1, buffer, 6);
    if (ret != 0) {
        printk("Error reading MAX86141 data!\n");
    }
    return ret;
}
