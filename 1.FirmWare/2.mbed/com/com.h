#pragma once
#include "mbed.h"

class COM{
public:
    COM(PinName SDA,PinName SCL);
    bool readable(void);
    uint8_t read(void);
    void i2c_start();
    void i2c_stop();
    void i2c_write_bit(int bit);
    int i2c_read_bit();
    void i2c_write_byte(int byte);
    int i2c_read_byte(bool ack);

    DigitalInOut* SDA;
    DigitalInOut* SCL;
};