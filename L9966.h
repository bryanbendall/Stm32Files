#pragma once

#include "stdint.h"

class L9966 {

public:
    enum class VoltageRange : uint8_t {
        FiveVolt = 0b00,
        TwentyVolt = 0b01,
        FortyVolt = 0b10,
        OneAndQuarterVolt = 0b11
    };

    enum class ResistanceRange : uint8_t {
        NoPullup = 0b00,
        RR1 = 0b01, // 180 ohm - measure aprox 50 to 2K
        RR2 = 0b10, // 9k ohm - measure aprox 2K to 30K
        RR3 = 0b11 // 50k ohm - measure aprox 30K to 400K
    };

    enum class IoPull : uint8_t {
        TwelveVolt = 0b001,
        FiveVolt = 0b010, // IO 13-15 redirected to 12v
        VVAR = 0b011,
        Ground = 0b100
    };

    enum class PullCurrent : uint8_t {
        uA7 = 0b000, // Up
        uA600 = 0b000, // Down
        uA20 = 0b001,
        uA100 = 0b010, // Down
        uA250 = 0b010, // Up
        uA500 = 0b011,
        mA1 = 0b100,
        mA5 = 0b101,
        mA10 = 0b110,
        mA20 = 0b111
    };

    static void init();
    static void setIoPull(uint16_t index, IoPull pull, PullCurrent current);
    static float readIoVoltage(uint16_t index, VoltageRange range);
    static float readIoResistance(uint16_t index, ResistanceRange range);
    static bool readIoDigital(uint16_t index);

private:
    static void setSlavePin(bool state);
    static void setSyncPin(bool state);
    static void setResetPin(bool state);
    static uint16_t readRegister(uint16_t reg);
    static void writeRegister(uint16_t reg, uint16_t data);
    static void softwareReset();
};