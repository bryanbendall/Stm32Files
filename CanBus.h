#pragma once

#include "Can/EBrytecCan.h"

class CanBus {

public:
    static void start();
    static void send(const Brytec::CanExtFrame& frame);
};