#pragma once

#include "gpio.h"
#include "tim.h"
#include <stdint.h>

template <uint32_t timerBase, uint32_t channel>
class Stm32TimerPwmOutput {

public:
    static void enable()
    {
        setPercent(0.0f);
        HAL_TIM_PWM_Start(getTimer(), channel);
    }

    static void disable()
    {
        setPercent(0.0f);
        HAL_TIM_PWM_Stop(getTimer(), channel);
    }

    static void setPercent(float percent)
    {
        TIM_TypeDef* timer = ((TIM_TypeDef*)timerBase);

        uint32_t ccr = 0;

        if (percent >= 0.99f)
            ccr = timer->ARR;
        else if (percent <= 0.01f)
            ccr = 0;
        else
            ccr = ((float)timer->ARR * percent);

        setCCR(ccr);
    }

    Stm32TimerPwmOutput() = delete;

private:
    static TIM_HandleTypeDef* getTimer()
    {
#ifndef NO_TIMER_1
        if constexpr (timerBase == TIM1_BASE)
            return &htim1;
#endif

#ifndef NO_TIMER_2
        if constexpr (timerBase == TIM2_BASE)
            return &htim2;
#endif

#ifndef NO_TIMER_3
        if constexpr (timerBase == TIM3_BASE)
            return &htim3;
#endif

#ifndef NO_TIMER_4
        if constexpr (timerBase == TIM4_BASE)
            return &htim4;
#endif

#ifndef NO_TIMER_5
        if constexpr (timer == 5)
            return &htim5;
#endif

#ifndef NO_TIMER_6
        if constexpr (timer == 6)
            return &htim6;
#endif

#ifndef NO_TIMER_7
        if constexpr (timer == 7)
            return &htim7;
#endif

#ifndef NO_TIMER_8
        if constexpr (timerBase == TIM8_BASE)
            return &htim8;
#endif
    }

    static void setCCR(uint32_t value)
    {
        TIM_TypeDef* timer = ((TIM_TypeDef*)timerBase);

        if constexpr (channel == TIM_CHANNEL_1)
            timer->CCR1 = value;

        if constexpr (channel == TIM_CHANNEL_2)
            timer->CCR2 = value;

        if constexpr (channel == TIM_CHANNEL_3)
            timer->CCR3 = value;

        if constexpr (channel == TIM_CHANNEL_4)
            timer->CCR4 = value;

        if constexpr (channel == TIM_CHANNEL_5)
            timer->CCR5 = value;

        if constexpr (channel == TIM_CHANNEL_6)
            timer->CCR6 = value;
    }
};
