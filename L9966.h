#pragma once

#include "stdint.h"

class L9966 {

public:
    // tests
    static float readVolts(uint8_t channel);
    static void readTest();

    static void init();
    static float readIoVoltage(uint16_t index);

private:
    static void setSlavePin(bool state);
    static void setSyncPin(bool state);
    static void setResetPin(bool state);
    static uint16_t readRegister(uint16_t reg);
    static void writeRegister(uint16_t reg, uint16_t data);
    static void softwareReset();
};