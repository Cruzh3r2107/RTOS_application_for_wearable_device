#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
// Define constants for sensor sampling rates
#define IMU_SAMPLE_RATE_HZ 50  // IMU sampling rate: 50 Hz
#define PPG_SAMPLE_RATE_HZ 100 // PPG sampling rate: 100 Hz
// Define the session structure to hold sensor data
typedef struct {
    uint32_t session_id;
    uint32_t timestamp;
    float imu_data[50 * 10]; // 10 seconds of IMU data at 50 Hz
    float ppg_data[100 * 10]; // 10 seconds of PPG data at 100 Hz
} session_t;

// Function prototypes for sensor interaction
void sensor_task(void *pvParameters); // Main sensor task
void read_imu_data(float *data, size_t len); // Read IMU data
void read_ppg_data(float *data, size_t len); // Read PPG data

#endif // SENSOR_DATA_H