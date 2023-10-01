#pragma once

#include <stdint.h>

class PwmDriver {

    enum class Register {
        MODE1 = 0x00,
        MODE2 = 0x01,
        SUBADR1 = 0x02,
        SUBADR2 = 0x03,
        SUBADR3 = 0x04,
        ALLCALLADR = 0x05,
        LED0_ON_L = 0x06,
        LED0_ON_H = 0x07,
        LED0_OFF_L = 0x08,
        LED0_OFF_H = 0x09,

        ALLLED_ON_L = 0xFA,
        ALLLED_ON_H = 0xFB,
        ALLLED_OFF_L = 0xFC,
        ALLLED_OFF_H = 0xFD,
        PRESCALE = 0xFE,
        TESTMODE = 0xFF
    };

    enum class Data {
        MODE1_ALLCAL = 0x01,
        MODE1_SUB3 = 0x02,
        MODE1_SUB2 = 0x04,
        MODE1_SUB1 = 0x08,
        MODE1_SLEEP = 0x10,
        MODE1_AI = 0x20,
        MODE1_EXTCLK = 0x40,
        MODE1_RESTART = 0x80,
        MODE2_OUTNE_0 = 0x01,
        MODE2_OUTNE_1 = 0x02,
        MODE2_OUTDRV = 0x04,
        MODE2_OCH = 0x08,
        MODE2_INVRT = 0x10
    };

    static constexpr uint16_t m_slaveAddress = 0x40 << 1;

public:
    static void init();
    static void sleep();
    static void wake();
    // values 0 - 4095, 4096 fully on and ignore off number
    static void setPwm(uint8_t pinNum, uint16_t on, uint16_t off);
    static void setOn(uint8_t pinNum);
    static void setOff(uint8_t pinNum);
    static void setEnableOutput(bool state);

private:
    PwmDriver() = default;
    static void sendCommand(Register reg, Data data);
    static void sendCommand(Register reg, uint8_t data);
    static uint8_t read(Register reg);
    static void reset();
    static void setPwmFrequency(float frequency);
    static void setOutputMode(bool totempole);
    static void setOePin(bool state);
};