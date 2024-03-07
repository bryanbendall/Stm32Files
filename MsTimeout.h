#pragma once

#include <stdint.h>

class MsTimeout {

public:
    MsTimeout(uint32_t timeoutMs);
    bool isTimeout();
    float getTimestep();

private:
    uint32_t m_timeout = 0;
    uint32_t m_lastMs = 0;
    float m_timestep = 0.0f;
};