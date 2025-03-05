#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <zephyr.h>
#include <drivers/i2c.h>

int read_ppg_data(uint8_t *buffer);
int read_imu_data(uint8_t *buffer);

#endif // SENSOR_DATA_H
