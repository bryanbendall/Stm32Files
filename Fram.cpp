#include "Fram.h"

#include "gpio.h"
#include "spi.h"

#include "stdio.h"

namespace FRAM {
constexpr uint8_t WREN = 0b000'0110;
constexpr uint8_t WRDI = 0b000'0100;
constexpr uint8_t RDSR = 0b000'0101;
constexpr uint8_t WRSR = 0b000'0001;
constexpr uint8_t READ = 0b000'0011;
constexpr uint8_t WRITE = 0b000'0010;
}

bool Fram::read(uint16_t address, uint8_t* destination, uint16_t size)
{
    if (!checkAddress(address, size))
        return false;

    setSlavePin(false);

    // Command
    uint8_t command = FRAM::READ;
    HAL_SPI_Transmit(&hspi1, &command, 1, HAL_MAX_DELAY);

    // Address
    // HAL_SPI_Transmit(&hspi1, (uint8_t*)&address, 2, HAL_MAX_DELAY);
    transferAddress(address);

    // Data
    HAL_SPI_Receive(&hspi1, destination, size, HAL_MAX_DELAY);

    setSlavePin(true);

    return true;
}

bool Fram::write(uint16_t address, uint8_t* source, uint16_t size)
{
    if (!checkAddress(address, size))
        return false;

    setWriteEnableLatch();

    setSlavePin(false);

    // Command
    uint8_t command = FRAM::WRITE;
    HAL_SPI_Transmit(&hspi1, &command, 1, HAL_MAX_DELAY);

    // Address
    // HAL_SPI_Transmit(&hspi1, (uint8_t*)&address, 2, HAL_MAX_DELAY);
    transferAddress(address);

    // Data
    HAL_SPI_Transmit(&hspi1, source, size, HAL_MAX_DELAY);

    setSlavePin(true);

    resetWriteEnableLatch();

    return true;
}

void Fram::setSlavePin(bool state)
{
    HAL_GPIO_WritePin(Memory_SS_GPIO_Port, Memory_SS_Pin, (GPIO_PinState)state);
}

void Fram::setWriteProtecPin(bool state)
{
    HAL_GPIO_WritePin(Memory_Wp_GPIO_Port, Memory_Wp_Pin, (GPIO_PinState)state);
}

void Fram::setHoldPin(bool state)
{
    HAL_GPIO_WritePin(Memory_Hold_GPIO_Port, Memory_Hold_Pin, (GPIO_PinState)state);
}

void Fram::setWriteEnableLatch()
{
    setSlavePin(false);

    // Command
    uint8_t command = FRAM::WREN;
    HAL_SPI_Transmit(&hspi1, &command, 1, HAL_MAX_DELAY);

    setSlavePin(true);
}

void Fram::resetWriteEnableLatch()
{
    setSlavePin(false);

    // Command
    uint8_t command = FRAM::WRDI;
    HAL_SPI_Transmit(&hspi1, &command, 1, HAL_MAX_DELAY);

    setSlavePin(true);
}

void Fram::transferAddress(uint16_t address)
{
    uint8_t addr[2];
    addr[0] = (address >> 8) & 0xFF;
    addr[1] = address & 0xFF;
    HAL_SPI_Transmit(&hspi1, addr, 2, HAL_MAX_DELAY);
}

bool Fram::checkAddress(uint16_t address, uint16_t size)
{
    return (address + size) < maxSize;
}
