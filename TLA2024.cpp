#include "TLA2024.h"

#include "UsDelay.h"
#include <stdio.h>

void TLA2024::init()
{
    printConfig();
    setRange(TLA2024::Range::FSR_6_144V);
}

void TLA2024::setInput(Input input)
{
    uint16_t config = getConfig();
    config &= ~(0b111 << 12);
    config |= (input << 12);
    setConfig(config);
}

void TLA2024::setRange(Range range)
{
    uint16_t config = getConfig();
    config &= ~(0b111 << 9);
    config |= (range << 9);
    setConfig(config);
}

uint16_t TLA2024::readRaw()
{
    // Start conversion
    uint16_t config = getConfig();
    config |= (1 << 15);
    setConfig(config);

    while (!(config = getConfig() & (1 << 15)))
        DWT_Delay_us(100);

    uint16_t raw = readRegister(Register::Conversion);
    raw = raw >> 4;

    // If negative sign bit is set, set to 0
    if (raw & (1 << 11))
        raw = 0;

    return raw;
}

float TLA2024::scaleReading(uint16_t reading, Range range)
{
    switch (range) {
    case Range::FSR_6_144V:
        return reading * 0.003f;
    case Range::FSR_4_096V:
        return reading * 0.002f;
    case Range::FSR_2_048V:
        return reading * 0.001f;
    case Range::FSR_1_024V:
        return reading * 0.0005f;
    case Range::FSR_0_512V:
        return reading * 0.00025f;
    case Range::FSR_0_256V:
        return reading * 0.000125f;
    }

    return 0.0f;
}

uint16_t TLA2024::getConfig()
{
    return readRegister(Register::Configuration);
}

void TLA2024::setConfig(uint16_t config)
{
    writeRegister(Register::Configuration, config);
}

uint16_t TLA2024::readRegister(Register reg)
{
    HAL_I2C_Master_Transmit(&hi2c2, ADDR, (uint8_t*)&reg, 1, HAL_MAX_DELAY);
    uint8_t raw[2];
    HAL_I2C_Master_Receive(&hi2c2, ADDR, raw, 2, HAL_MAX_DELAY);

    uint16_t data = raw[0] << 8 | raw[1];
    return data;
}

void TLA2024::writeRegister(Register reg, uint16_t data)
{
    uint8_t raw[3];
    raw[0] = reg;
    raw[1] = (data >> 8) & 0xFF;
    raw[2] = data & 0xFF;

    HAL_I2C_Master_Transmit(&hi2c2, ADDR, raw, 3, HAL_MAX_DELAY);
}

void TLA2024::printConfig()
{
    uint16_t config = getConfig();
    printf("TLA config hex - %x\n", config);
}