#pragma once

#include "gpio.h"
#include <stdint.h>

template <uint32_t port, uint16_t pin>
class Stm32Output {

public:
    static void setState(bool state)
    {
        HAL_GPIO_WritePin((GPIO_TypeDef*)port, pin, (GPIO_PinState)state);
    }

    Stm32Output() = delete;
};