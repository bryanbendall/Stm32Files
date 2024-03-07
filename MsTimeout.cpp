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
        m_timestep = ((float)difference * 0.001f);
        m_lastMs = HAL_GetTick();
        return true;
    }

    return false;
}

float MsTimeout::getTimestep()
{
    return m_timestep;
}