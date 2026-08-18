#pragma once

#include "gpio.h"
#include "tim.h"
#include <stdint.h>

template <TIM_HandleTypeDef* timer, uint32_t channel>
class Stm32TimerPwmOutput {

public:
    static void enable()
    {
        setPercent(0.0f);
        HAL_TIM_PWM_Start(timer, channel);
    }

    static void disable()
    {
        setPercent(0.0f);
        HAL_TIM_PWM_Stop(timer, channel);
    }

    static void setPercent(float percent)
    {
        uint32_t ccr = 0;
        uint32_t arr = __HAL_TIM_GET_AUTORELOAD(timer);

        if (percent >= 0.99f)
            ccr = arr;
        else if (percent <= 0.01f)
            ccr = 0;
        else
            ccr = ((float)arr * percent);

        setCCR(ccr);
    }

    static void setCCR(uint32_t ccr)
    {
        // To replace following code
        // __HAL_TIM_SET_COMPARE(timer, channel, ccr);

        if constexpr (channel == TIM_CHANNEL_1)
            timer->Instance->CCR1 = ccr;
        if constexpr (channel == TIM_CHANNEL_2)
            timer->Instance->CCR2 = ccr;
        if constexpr (channel == TIM_CHANNEL_3)
            timer->Instance->CCR3 = ccr;
        if constexpr (channel == TIM_CHANNEL_4)
            timer->Instance->CCR4 = ccr;
        if constexpr (channel == TIM_CHANNEL_5)
            timer->Instance->CCR5 = ccr;
        if constexpr (channel == TIM_CHANNEL_6)
            timer->Instance->CCR6 = ccr;
    }

    Stm32TimerPwmOutput() = delete;
};
