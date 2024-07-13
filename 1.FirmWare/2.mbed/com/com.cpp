#include "com.h"

COM::COM(PinName SDA_pin,PinName SCL_pin)
{
    SDA = new DigitalInOut(SDA_pin);
    SCL = new DigitalInOut(SCL_pin);
}
void COM::i2c_start() {
    *SDA = 1;
    *SCL = 1;
    *SDA = 0;
    *SCL = 0;
}

void COM::i2c_stop() {
    *SDA = 0;
    *SCL = 1;
    *SDA = 1;
}

void COM::i2c_write_bit(int bit) {
    *SDA = bit;
    *SCL = 1;
    *SCL = 0;
}

int COM::i2c_read_bit() {
    *SCL = 1;
    int bit = *SDA;
    *SCL = 0;
    return bit;
}

void COM::i2c_write_byte(int byte) {
    for (int i = 7; i >= 0; i--) {
        i2c_write_bit((byte >> i) & 1);
    }
}

int COM::i2c_read_byte(bool ack) {
    int byte = 0;
    for (int i = 7; i >= 0; i--) {
        byte |= (i2c_read_bit() << i);
    }
    i2c_write_bit(!ack);
    return byte;
}