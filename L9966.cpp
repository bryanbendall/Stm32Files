#include "L9966.h"

#include "UsDelay.h"
#include "gpio.h"
#include "spi.h"
#include <stdio.h>

enum Register : uint32_t {
    GEN_STATUS = 0b0000'0001,
    DEV_V = 0b0000'0010,
    HW_REV = 0b0000'0011,
    DEV_ID = 0b0000'0100,
    CURR_SRC_CTRL_1 = 0b0010'0001,
    CURR_SRC_CTRL_2 = 0b0010'0010,
    CURR_SRC_CTRL_3 = 0b0010'0011,
    CURR_SRC_CTRL_4 = 0b0010'0100,
    CURR_SRC_CTRL_5 = 0b0010'0101,
    CURR_SRC_CTRL_6 = 0b0010'0110,
    CURR_SRC_CTRL_7 = 0b0010'0111,
    CURR_SRC_CTRL_8 = 0b0010'1000,
    CURR_SRC_CTRL_9 = 0b0010'1001,
    CURR_SRC_CTRL_10 = 0b0010'1010,
    CURR_SRC_CTRL_11 = 0b0010'1011,
    CURR_SRC_CTRL_12 = 0b0010'1100,
    CURR_SRC_CTRL_13 = 0b0010'1101,
    CURR_SRC_CTRL_14 = 0b0010'1110,
    CURR_SRC_CTRL_15 = 0b0010'1111,
    SWITCH_ROUTE = 0b0011'0000,
    DWT_VOLT_SRC_LSF_CTRL = 0b0011'0001,
    DIG_IN_STAT_LTC = 0b0011'0011,
    GTM_TO_SENT_ROUTE_1_2 = 0b0011'0100,
    GTM_TO_SENT_ROUTE_3_4 = 0b0011'0101,
    ACTIVE_DISCHARGE_LSF_CTRL = 0b0011'0110,
    WAK_MSK = 0b0100'0000,
    SLEEP_CONFIG = 0b0100'0001,
    WAK_CONFIG = 0b0100'0010,
    SOFT_RST_CMD = 0b0100'0011,
    VRS = 0b0101'0001,
    SQNCR_INT_MSK_FLG = 0b1000'0000,
    SC_CONF = 0b1000'0001,
    ADC_TIMING = 0b1000'0010,
    SC_RESULT = 0b1000'0011,
    SQNCR_CMD_1 = 0b1100'0001,
    SQNCR_CMD_2 = 0b1100'0010,
    SQNCR_CMD_3 = 0b1100'0011,
    SQNCR_CMD_4 = 0b1100'0100,
    SQNCR_CMD_5 = 0b1100'0101,
    SQNCR_CMD_6 = 0b1100'0110,
    SQNCR_CMD_7 = 0b1100'0111,
    SQNCR_CMD_8 = 0b1100'1000,
    SQNCR_CMD_9 = 0b1100'1001,
    SQNCR_CMD_10 = 0b1100'1010,
    SQNCR_CMD_11 = 0b1100'1011,
    SQNCR_CMD_12 = 0b1100'1100,
    SQNCR_CMD_13 = 0b1100'1101,
    SQNCR_CMD_14 = 0b1100'1110,
    SQNCR_CMD_15 = 0b1100'1111,
    SQNCR_CTRL = 0b1101'0000,
    SQNCR_RSLT_COPY_CMD = 0b1101'1111,
    DIG_IN_STAT = 0b1110'0000,
    SQNCR_RESULT_1 = 0b1110'0001,
    SQNCR_RESULT_2 = 0b1110'0010,
    SQNCR_RESULT_3 = 0b1110'0011,
    SQNCR_RESULT_4 = 0b1110'0100,
    SQNCR_RESULT_5 = 0b1110'0101,
    SQNCR_RESULT_6 = 0b1110'0110,
    SQNCR_RESULT_7 = 0b1110'0111,
    SQNCR_RESULT_8 = 0b1110'1000,
    SQNCR_RESULT_9 = 0b1110'1001,
    SQNCR_RESULT_10 = 0b1110'1010,
    SQNCR_RESULT_11 = 0b1110'1011,
    SQNCR_RESULT_12 = 0b1110'1100,
    SQNCR_RESULT_13 = 0b1110'1101,
    SQNCR_RESULT_14 = 0b1110'1110,
    SQNCR_RESULT_15 = 0b1110'1111
};

void L9966::init()
{
    setResetPin(true);
    HAL_Delay(10);
    uint16_t genReg = readRegister(Register::GEN_STATUS);
    if (genReg & 0b0110)
        printf("L9966 Init Failed\n");

    HAL_Delay(10);
    // Calibrate
    uint16_t cal = 0;
    cal |= (1 << 6);
    writeRegister(Register::GEN_STATUS, cal);
    HAL_Delay(10);

    uint16_t timingData = 0;
    timingData |= (10 << 12); // Adc1 - 80us
    timingData |= (1 << 0); // RR1 - 200us
    timingData |= (2 << 4); // RR2 - 400us
    timingData |= (9 << 8); // RR3 - 1.8ms
    writeRegister(Register::ADC_TIMING, timingData);
}

void L9966::setIoPull(uint16_t index, IoPull pull, PullCurrent current)
{
    if (index > 15)
        return;

    uint16_t reg = 0;
    reg |= ((uint8_t)pull << 1);
    reg |= ((uint8_t)current << 6);

    writeRegister(Register::CURR_SRC_CTRL_1 + (index - 1), reg);
}

float L9966::readIoVoltage(uint16_t index, VoltageRange range)
{
    if (index == 0 || index > 12)
        return 0.0f;

    // Single conversion
    uint16_t scConfData = 0;
    scConfData |= (1 << 0); // Voltage select
    scConfData |= (((uint8_t)range & 0x3) << 1); // Range
    scConfData |= (index << 3); // Select io
    scConfData |= (1 << 8); // ADC_RUN
    writeRegister(Register::SC_CONF, scConfData);

    uint16_t scResult = 0;
    while ((scResult & (1 << 14)) == 0) {
        scResult = readRegister(Register::SC_RESULT);
    }

    float result = (float)(scResult & 0xFFF) / 4095.0f;

    switch (range) {
    case VoltageRange::FiveVolt:
        return result * 5.0f;
    case VoltageRange::TwentyVolt:
        return result * 20.0f;
    case VoltageRange::FortyVolt:
        return result * 40.0f;
    case VoltageRange::OneAndQuarterVolt:
        return result * 1.25f;

    default:
        return 0.0f;
    }
}

float L9966::readIoResistance(uint16_t index, ResistanceRange range)
{
    if (index == 0 || index > 12)
        return 0.0f;

    // Single conversion
    uint16_t scConfData = 0;
    scConfData |= (((uint8_t)range & 0x3) << 1); // Range
    scConfData |= (index << 3); // Select io
    scConfData |= (1 << 8); // ADC_RUN
    writeRegister(Register::SC_CONF, scConfData);

    uint16_t scResult = 0;
    while ((scResult & 0x7FFF) == 0) {
        scResult = readRegister(Register::SC_RESULT);
    }

    uint16_t resistance = 0;
    switch (range) {
    case ResistanceRange::NoPullup:
        resistance = 0;
        break;
    case ResistanceRange::RR1:
        resistance = 180;
        break;
    case ResistanceRange::RR2:
        resistance = 9'100;
        break;
    case ResistanceRange::RR3:
        resistance = 50'000;
        break;
    default:
        break;
    }

    return (float)(scResult & 0x7FFF) / 2048.0f * (float)resistance;
}

bool L9966::readIoDigital(uint16_t index)
{
    uint16_t reg = readRegister(Register::DIG_IN_STAT);
    return reg & (1 << (index - 1));
}

void L9966::setSlavePin(bool state)
{
    HAL_GPIO_WritePin(SPI1_SS_L9966_GPIO_Port, SPI1_SS_L9966_Pin, (GPIO_PinState)state);
}

void L9966::setSyncPin(bool state)
{
    HAL_GPIO_WritePin(L6699_Sync_GPIO_Port, L6699_Sync_Pin, (GPIO_PinState)state);
}

void L9966::setResetPin(bool state)
{
    HAL_GPIO_WritePin(L6699_Rst_GPIO_Port, L6699_Rst_Pin, (GPIO_PinState)state);
}

bool getPairty(uint32_t n)
{
    bool parity = 0;
    while (n) {
        parity = !parity;
        n = n & (n - 1);
    }
    return parity;
}

void checkFixedPattern(uint16_t data)
{
    // if ((data & 0x3E00) != 0x2A00)
    // printf("Fixed pattern BAD\n");
}

uint16_t L9966::readRegister(uint16_t reg)
{
    uint16_t sendData[2] = {};
    sendData[0] |= (1 << 15);
    sendData[0] |= (1 << 14); // CTRL_CFG
    sendData[0] |= (1 << 12); // Not burst mode
    sendData[0] |= (reg & 0xFF) << 4; // 7 bit address

    // Command pairty
    if (!getPairty(sendData[0]))
        sendData[0] |= (1 << 0);

    // Data pairty
    sendData[1] = 0;
    if (!getPairty(sendData[1]))
        sendData[1] |= (1 << 15);

    setSlavePin(false);
    DWT_Delay_us(100);

    uint16_t readData[2] = {};
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)sendData, (uint8_t*)readData, 2, HAL_MAX_DELAY);

    DWT_Delay_us(100);
    setSlavePin(true);

    checkFixedPattern(readData[0]);

    return readData[1] & 0x7FFF;
}

void L9966::writeRegister(uint16_t reg, uint16_t data)
{
    uint16_t sendData[2] = {};
    sendData[0] |= (1 << 15);
    sendData[0] |= (1 << 14); // CTRL_CFG
    sendData[0] |= (1 << 13); // Write
    sendData[0] |= (1 << 12); // Not burst mode
    sendData[0] |= (reg & 0xFF) << 4;

    // Command Pairty
    if (!getPairty(sendData[0]))
        sendData[0] |= (1 << 0);

    // Data Pairty
    sendData[1] = data & 0x7FFF;
    if (!getPairty(sendData[1]))
        sendData[1] |= (1 << 15);

    setSlavePin(false);
    DWT_Delay_us(100);

    uint16_t readData[2] = {};
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)sendData, (uint8_t*)readData, 2, HAL_MAX_DELAY);

    checkFixedPattern(readData[0]);

    DWT_Delay_us(100);
    setSlavePin(true);
}

void L9966::softwareReset()
{
    writeRegister(Register::SOFT_RST_CMD, 0b1001);
    writeRegister(Register::SOFT_RST_CMD, 0b0110);
    writeRegister(Register::SOFT_RST_CMD, 0b0011);
}
