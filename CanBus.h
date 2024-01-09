#pragma once

#include "Can/EBrytecCan.h"
#include "ECanBus.h"

class CanBus {

public:
    static void start(uint8_t index, Brytec::CanSpeed::Types speed);
    static void send(uint8_t index, const Brytec::CanFrame& frame);
};