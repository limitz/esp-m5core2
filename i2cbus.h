#pragma once
#include <stdint.h>

int i2cbus_init(int bus, int sda, int scl, int speed);
int i2cbus_deinit(int bus);
int i2c_write_reg(int bus, int addr, int reg, const uint8_t* data, int len);
int i2c_read_reg(int bus, int addr, int reg, uint8_t* data, int len);
