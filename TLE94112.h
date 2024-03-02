#pragma once

#include "spi.h"
#include "stdint.h"

class TLE94112 {

public:
    enum OutputDrive : uint8_t {
        LowSide = 0,
        HighSide = 1,
        Off
    };

public:
    static void init();
    static void setOutput(uint8_t index, OutputDrive type);
    static void setAllOff();
    static bool isOverCurrent(uint8_t index, bool reset = false);
    static void resetOverCurrentFlag(uint8_t index);

private:
    static void setSlavePin(bool state);
    static void setEnablePin(bool state);
    static uint8_t readRegister(uint8_t reg);
    static void writeRegister(uint8_t reg, uint8_t data);
    static void printStatus();
};