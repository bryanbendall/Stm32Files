#pragma once

#include "IOTypes.h"
#include "L9966.h"

template <typename Input>
class BoardPinInput {

public:
    static void setupType(Brytec::IOTypes::Types type)
    {
        switch (type) {
        case Brytec::IOTypes::Types::Input_Ground:
            Input::setPullup();
            break;
        case Brytec::IOTypes::Types::Input_5V:
        case Brytec::IOTypes::Types::Input_Batt:
            Input::setPulldown();
            break;

        default:
            break;
        }
    }

    static float getValue(Brytec::IOTypes::Types type)
    {
        switch (type) {
        case Brytec::IOTypes::Types::Input_Ground:
            return !Input::getDigitalState();

        case Brytec::IOTypes::Types::Input_5V:
        case Brytec::IOTypes::Types::Input_Batt:
            return Input::getDigitalState();

        case Brytec::IOTypes::Types::Input_5V_Variable:
            return Input::readVoltage(L9966::VoltageRange::FiveVolt);

        case Brytec::IOTypes::Types::Input_20V_Variable:
            return Input::readVoltage(L9966::VoltageRange::TwentyVolt);

        case Brytec::IOTypes::Types::Input_Resistance:
            return Input::readResistance();

        default:
            break;
        }

        return 0.0f;
    }
};