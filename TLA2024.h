#pragma once

#include "i2c.h"

class TLA2024 {

public:
    enum Input : uint8_t {
        AIN0 = 0b100,
        AIN1 = 0b101,
        AIN2 = 0b110,
        AIN3 = 0b111
    };

    enum Range : uint8_t {
        FSR_6_144V = 0b000,
        FSR_4_096V = 0b001,
        FSR_2_048V = 0b010,
        FSR_1_024V = 0b011,
        FSR_0_512V = 0b100,
        FSR_0_256V = 0b101
    };

public:
    static void init();
    static void setInput(Input input);
    static void setRange(Range range);
    static uint16_t readRaw();
    static float scaleReading(uint16_t reading, Range range);

private:
    enum Register : uint8_t {
        Conversion = 0x00,
        Configuration = 0x01
    };

private:
    static uint16_t getConfig();
    static void setConfig(uint16_t config);
    static uint16_t readRegister(Register reg);
    static void writeRegister(Register reg, uint16_t data);
    static void printConfig();

private:
    static const uint8_t ADDR = 0b1001'0000;
};