#include "TLE94112.h"

#include "UsDelay.h"
#include <stdio.h>

enum ControlRegister : uint8_t {
    HB_ACT_1_CTRL = 0b000'0011,
    HB_ACT_2_CTRL = 0b100'0011,
    HB_ACT_3_CTRL = 0b010'0011,
    HB_MODE_1_CTRL = 0b110'0011,
    HB_MODE_2_CTRL = 0b001'0011,
    HB_MODE_3_CTRL = 0b101'0011,
    PWM_CH_FREQ_CTRL = 0b011'0011,
    PWM1_DC_CTRL = 0b111'0011,
    PWM2_DC_CTRL = 0b000'1011,
    PWM3_DC_CTRL = 0b100'1011,
    FW_OL_CTRL = 0b010'1011,
    FW_CTRL = 0b110'1011,
    CONFIG_CTRL = 0b110'0111
};

enum StatusRegister : uint8_t {
    SYS_DIAG_1 = 0b001'1011,
    SYS_DIAG_2 = 0b101'1011,
    SYS_DIAG_3 = 0b011'1011,
    SYS_DIAG_4 = 0b111'1011,
    SYS_DIAG_5 = 0b000'0111,
    SYS_DIAG_6 = 0b100'0111,
    SYS_DIAG_7 = 0b010'0111
};

enum ControlBit : uint8_t {
    READ = 0b0000'0000,
    WRITE_CLEAR = 0b1000'0000
};

void TLE94112::init()
{
    setEnablePin(true);
    DWT_Delay_us(250);
}

void TLE94112::setOutput(uint8_t index, OutputDrive type, bool immediate)
{

    if (index < 1 || index > 12)
        return;

    uint8_t regAddr = 0;
    uint8_t shift = ((index - 1) * 2) % 8;

    uint8_t reg;

    if (index > 8) {
        // 9 - 12
        regAddr = HB_ACT_3_CTRL;
        reg = s_HB_ACT_3_CTRL;
    } else if (index > 4) {
        // 5 - 8
        regAddr = HB_ACT_2_CTRL;
        reg = s_HB_ACT_2_CTRL;

    } else {
        // 1 - 4
        regAddr = HB_ACT_1_CTRL;
        reg = s_HB_ACT_1_CTRL;
    }

    reg &= !((1 << (shift + OutputDrive::LowSide)) | (1 << (shift + OutputDrive::HighSide))); // clear bits

    if (type != OutputDrive::Off)
        reg |= (1 << (shift + type));

    if (immediate) {
        if (type == OutputDrive::Off)
            resetOverCurrentFlag(index);

        writeRegister(regAddr, reg);
    }
}

void TLE94112::setAllOutputs()
{
    writeRegister(HB_ACT_1_CTRL, s_HB_ACT_1_CTRL);
    writeRegister(HB_ACT_2_CTRL, s_HB_ACT_2_CTRL);
    writeRegister(HB_ACT_3_CTRL, s_HB_ACT_3_CTRL);
}

void TLE94112::setAllOff()
{
    s_HB_ACT_1_CTRL = 0;
    s_HB_ACT_2_CTRL = 0;
    s_HB_ACT_3_CTRL = 0;

    setAllOutputs();
}

bool TLE94112::isOverCurrent(uint8_t index, bool reset)
{
    if (index < 1 || index > 12)
        return false;

    uint8_t regAddr = 0;
    uint8_t shift = ((index - 1) * 2) % 8;

    if (index > 8) // 9 - 12
        regAddr = StatusRegister::SYS_DIAG_4;
    else if (index > 4) // 5 - 8
        regAddr = StatusRegister::SYS_DIAG_3;
    else // 1 - 4
        regAddr = StatusRegister::SYS_DIAG_2;

    uint8_t status = readRegister(regAddr);

    if(reset){
        uint8_t resetStatus = status & !(1 << shift | 1 << (shift + 1));
        writeRegister(regAddr, resetStatus);
    }

    return status & (1 << shift | 1 << (shift + 1));
}

void TLE94112::resetOverCurrentFlag(uint8_t index)
{
    isOverCurrent(index, true);
}

void TLE94112::setSlavePin(bool state)
{
    HAL_GPIO_WritePin(SPI2_SS_TLE94112_GPIO_Port, SPI2_SS_TLE94112_Pin, (GPIO_PinState)state);
}

void TLE94112::setEnablePin(bool state)
{
    HAL_GPIO_WritePin(TLE94112_EN_GPIO_Port, TLE94112_EN_Pin, (GPIO_PinState)state);
}

uint8_t TLE94112::readRegister(uint8_t reg)
{
    uint8_t sendData[2] = {};
    sendData[0] = reg;

    setSlavePin(false);
    DWT_Delay_us(100);

    uint8_t readData[2] = {};
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)sendData, (uint8_t*)readData, 2, HAL_MAX_DELAY);

    DWT_Delay_us(100);
    setSlavePin(true);

    return readData[1];
}

void TLE94112::writeRegister(uint8_t reg, uint8_t data)
{
    uint8_t sendData[2] = {};
    sendData[0] = reg | ControlBit::WRITE_CLEAR;
    sendData[1] = data;

    setSlavePin(false);
    DWT_Delay_us(100);

    uint8_t readData[2] = {};
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)sendData, (uint8_t*)readData, 2, HAL_MAX_DELAY);

    DWT_Delay_us(100);
    setSlavePin(true);
}

void TLE94112::printStatus()
{
    printf("Diag 1 - %d\n", readRegister(StatusRegister::SYS_DIAG_1));
    printf("Diag 2 - %d\n", readRegister(StatusRegister::SYS_DIAG_2));
    printf("Diag 3 - %d\n", readRegister(StatusRegister::SYS_DIAG_3));
    printf("Diag 4 - %d\n", readRegister(StatusRegister::SYS_DIAG_4));
    printf("Diag 5 - %d\n", readRegister(StatusRegister::SYS_DIAG_5));
    printf("Diag 6 - %d\n", readRegister(StatusRegister::SYS_DIAG_6));
    printf("Diag 7 - %d\n", readRegister(StatusRegister::SYS_DIAG_7));
}