#pragma once

#include "PwmDriver.h"
#include <stdint.h>

template <int channel>
class PwmDriverOutput {

public:
    static void setState(bool state)
    {
        if (state)
            PwmDriver::setOn(channel);
        else
            PwmDriver::setOff(channel);
    }

    static void setPwm(float value)
    {
        uint16_t on = (uint16_t)(4096.0f * value);
        // uint16_t off = (uint16_t)(4096.0f * (1.0f - value));

        PwmDriver::setPwm(channel, 0, on);
    }

    PwmDriverOutput() = delete;
};