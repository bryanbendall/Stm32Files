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

        __HAL_TIM_SET_COMPARE(timer, channel, ccr);
    }

    Stm32TimerPwmOutput() = delete;
};
