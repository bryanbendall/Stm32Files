#pragma once

#include <stdint.h>

class MsTimeout {

public:
    MsTimeout(uint32_t timeoutMs);
    bool isTimeout();
    uint32_t getTimestepMs();

private:
    uint32_t m_timeout = 0;
    uint32_t m_lastMs = 0;
    uint32_t m_timestepMs = 0;
};