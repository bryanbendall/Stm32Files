#include "MsTimeout.h"

#include "stm32g4xx_hal.h"

MsTimeout::MsTimeout(uint32_t timeoutMs)
    : m_timeout(timeoutMs)
{
}

bool MsTimeout::isTimeout()
{
    uint32_t difference = HAL_GetTick() - m_lastMs;
    if (difference >= m_timeout) {
        m_timestepMs = difference;
        m_lastMs = HAL_GetTick();
        return true;
    }

    return false;
}

uint32_t MsTimeout::getTimestepMs()
{
    return m_timestepMs;
}