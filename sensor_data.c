#include "sensor_data.h"
#include <sys/printk.h>

#define MPU6050_I2C_ADDR  0x68  // IMU sensor address
#define MAX86141_I2C_ADDR 0x55  // PPG sensor address

// Define message queue for sensor data
K_MSGQ_DEFINE(sensor_data_msgq, sizeof(struct sensor_session), 10, 4);

// Sensor task: Reads sensor data and pushes to message queue
void sensor_task(void) {
    uint8_t imu_buffer[14];  // MPU6050 returns 14 bytes (Accel + Gyro + Temp)
    uint8_t ppg_buffer[6];   // MAX86141 returns 6 bytes

    while (1) {
        struct sensor_session session;
        session.timestamp = k_uptime_get_32();  // Get system timestamp

        // Read IMU data
        if (read_mpu6050_data(imu_buffer) != 0) {
            printk("IMU read failed. Stopping session.\n");
            break;
        }

        // Read PPG data
        if (read_max86141_data(ppg_buffer) != 0) {
            printk("PPG read failed. Stopping session.\n");
            break;
        }

        // Parse IMU Data (MPU6050)
        session.acc_x = (imu_buffer[0] << 8) | imu_buffer[1];
        session.acc_y = (imu_buffer[2] << 8) | imu_buffer[3];
        session.acc_z = (imu_buffer[4] << 8) | imu_buffer[5];
        session.gyr_x = (imu_buffer[8] << 8) | imu_buffer[9];
        session.gyr_y = (imu_buffer[10] << 8) | imu_buffer[11];
        session.gyr_z = (imu_buffer[12] << 8) | imu_buffer[13];

        // Parse PPG Data (MAX86141)
        session.red = (ppg_buffer[0] << 16) | (ppg_buffer[1] << 8) | ppg_buffer[2];
        session.ir = (ppg_buffer[3] << 16) | (ppg_buffer[4] << 8) | ppg_buffer[5];

        // Send data to message queue
        k_msgq_put(&sensor_data_msgq, &session, K_NO_WAIT);

        // 40ms sleep → 25Hz sampling rate (IMU: 25Hz, PPG: 50Hz)
        k_sleep(K_MSEC(40));
    }
}

// Read IMU data via I2C (MPU6050)
int read_mpu6050_data(uint8_t *buffer) {
    return i2c_write_read(i2c_dev, MPU6050_I2C_ADDR, (uint8_t[]){0x3B}, 1, buffer, 14);
}

// Read PPG data via I2C (MAX86141)
int read_max86141_data(uint8_t *buffer) {
    return i2c_write_read(i2c_dev, MAX86141_I2C_ADDR, (uint8_t[]){0x07}, 1, buffer, 6);
}
