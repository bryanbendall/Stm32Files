#pragma once

#include "spi.h"

class TMP126 {

    enum Register : uint8_t {
        tempResult = 0x00,
        slewResult = 0x01,
        alertStatus = 0x02,
        configuration = 0x03,
        alertEnable = 0x04,
        tLowLimit = 0x05,
        tHighLimit = 0x06,
        hysteresis = 0x07,
        slewLimit = 0x08,
        uniqueId1 = 0x09,
        uniqueId2 = 0x0A,
        uniqueId3 = 0x0B,
        deviceId = 0x0C
    };

public:
    static void init();
    static float getTemp();
    static uint16_t readId1();
    static uint16_t readId2();
    static uint16_t readId3();
    static uint16_t readDeviceId();

private:
    static void setSlavePin(bool state);
    static uint16_t readRegister(Register reg);
    static void writeRegister(Register reg, uint16_t data);
};