#pragma once

#include "Usb/UsbDefs.h"

class Usb {

public:
    Usb() = default;

    static void send(Brytec::UsbPacket& packet);
    static void update();
};