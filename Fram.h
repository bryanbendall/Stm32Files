#pragma once

#include "spi.h"
#include <stdint.h>

class Fram {

public:
    static void init(SPI_HandleTypeDef* spi);
    static bool read(uint16_t address, uint8_t* destination, uint16_t size);
    static bool write(uint16_t address, uint8_t* source, uint16_t size);

public:
    static constexpr uint16_t maxSize = 0x1FFF;

private:
    static void setSlavePin(bool state);
    static void setWriteProtecPin(bool state);
    static void setHoldPin(bool state);
    static void setWriteEnableLatch();
    static void resetWriteEnableLatch();
    static void transferAddress(uint16_t address);
    static bool checkAddress(uint16_t address, uint16_t size);

private:
    static SPI_HandleTypeDef* s_spi;
};