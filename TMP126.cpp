#include "TMP126.h"

#include "UsDelay.h"
#include <stdio.h>

void TMP126::init()
{
    setSlavePin(false);
    setSlavePin(true);
}

float TMP126::getTemp()
{
    uint16_t temp = readRegister(Register::tempResult);
    temp = temp >> 2;
    return (float)temp * 0.03125f;
}

uint16_t TMP126::readId1()
{
    return readRegister(Register::uniqueId1);
}

uint16_t TMP126::readId2()
{
    return readRegister(Register::uniqueId2);
}

uint16_t TMP126::readId3()
{
    return readRegister(Register::uniqueId3);
}

uint16_t TMP126::readDeviceId()
{
    return readRegister(Register::deviceId);
}

void TMP126::setSlavePin(bool state)
{
    HAL_GPIO_WritePin(SPI2_SS_TMP126_GPIO_Port, SPI2_SS_TMP126_Pin, (GPIO_PinState)state);
}

uint16_t TMP126::readRegister(Register reg)
{
    uint16_t command = 0;
    command |= reg;
    command |= (1 << 8); // Read bit

    setSlavePin(false);

    HAL_SPI_Transmit(&hspi2, (uint8_t*)&command, 1, HAL_MAX_DELAY);

    uint16_t recieve;
    HAL_SPI_Receive(&hspi2, (uint8_t*)&recieve, 1, HAL_MAX_DELAY);

    setSlavePin(true);

    return recieve;
}

void TMP126::writeRegister(Register reg, uint16_t data)
{
    uint16_t command[2] = { 0 };
    command[1] = data;

    setSlavePin(false);
    HAL_SPI_Transmit(&hspi2, (uint8_t*)command, 2, HAL_MAX_DELAY);
    setSlavePin(true);
}