#include "sensor_data.h"
#include <zephyr.h>
#include <device.h>
#include <drivers/i2c.h>
#include <sys/printk.h>

#define I2C_DEV_LABEL DT_LABEL(DT_NODELABEL(i2c0))
#define MAX30102_I2C_ADDR 0x57
#define BMI270_I2C_ADDR 0x68

static const struct device *i2c_dev = NULL;

static void init_i2c() {
    if (!i2c_dev) {
        i2c_dev = device_get_binding(I2C_DEV_LABEL);
        if (!i2c_dev) {
            printk("Failed to get I2C device binding\n");
        }
    }
}

int read_ppg_data(uint8_t *buffer) {
    init_i2c();
    int ret = i2c_write_read(i2c_dev, MAX30102_I2C_ADDR, (uint8_t[]){0x07}, 1, buffer, 6);
    if (ret != 0) {
        printk("PPG I2C read error: %d\n", ret);
    }
    return ret;
}

int read_imu_data(uint8_t *buffer) {
    init_i2c();
    int ret = i2c_write_read(i2c_dev, BMI270_I2C_ADDR, (uint8_t[]){0x0C}, 1, buffer, 12);
    if (ret != 0) {
        printk("IMU I2C read error: %d\n", ret);
    }
    return ret;
}
