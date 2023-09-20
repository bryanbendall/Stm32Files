#pragma once

#include "EBrytecCan.h"
#include "Usb/UsbDefs.h"

class Usb {

public:
    Usb() = default;

    static void send(Brytec::CanExtFrame& frame);
    static void send(Brytec::UsbPacket& packet);
    static void update();
};