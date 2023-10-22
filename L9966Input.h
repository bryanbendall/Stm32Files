#pragma once

#include "L9966.h"
#include <stdint.h>

template <int channel>
class L9966Input {

public:
    static void setPullup()
    {
        L9966::setIoPull(channel, L9966::IoPull::FiveVolt, L9966::PullCurrent::mA1);
    }

    static void setPulldown()
    {
        L9966::setIoPull(channel, L9966::IoPull::Ground, L9966::PullCurrent::mA1);
    }

    static bool getDigitalState()
    {
        return L9966::readIoDigital(channel);
    }

    static float readVoltage(L9966::VoltageRange range)
    {
        return L9966::readIoVoltage(channel, range);
    }

    static float readResistance()
    {
        return L9966::readIoResistanceAutorange(channel);
    }

    L9966Input() = delete;
};