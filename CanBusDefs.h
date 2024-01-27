#pragma once

#include "fdcan.h"
#include <stdint.h>

namespace CanBusDefs {

FDCAN_HandleTypeDef* getCanBus(uint8_t index);
uint8_t getCanBusIndex(FDCAN_HandleTypeDef* bus);

}