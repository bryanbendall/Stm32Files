#include "PwmDriver.h"

#include "gpio.h"
#include "i2c.h"

#include <stdio.h>

void PwmDriver::init()
{
    reset();
    setPwmFrequency(300);
    setOutputMode(true);

    // Make sure all channels are off
    for (uint8_t i = 0; i < 16; i++)
        setOff(i);
}

void PwmDriver::sleep()
{
    uint8_t awake = read(Register::MODE1);
    uint8_t sleep = awake | (uint8_t)Data::MODE1_SLEEP; // set sleep bit high
    sendCommand(Register::MODE1, sleep);
    HAL_Delay(5); // wait until cycle ends for sleep to be active
}

void PwmDriver::wake()
{
    uint8_t sleep = read(Register::MODE1);
    uint8_t wakeup = sleep & ~(uint8_t)Data::MODE1_SLEEP; // set sleep bit low
    sendCommand(Register::MODE1, wakeup);
}

void PwmDriver::setPwm(uint8_t pinNum, uint16_t on, uint16_t off)
{
    if (pinNum > 15)
        return;

    uint8_t buff[] = {
        (uint8_t)((uint8_t)Register::LED0_ON_L + 4 * pinNum),
        (uint8_t)on,
        (uint8_t)(on >> 8),
        (uint8_t)off,
        (uint8_t)(off >> 8),
    };

    HAL_I2C_Master_Transmit(&hi2c2, m_slaveAddress, buff, 5, HAL_MAX_DELAY);
}

void PwmDriver::setOn(uint8_t pinNum)
{
    setPwm(pinNum, 4096, 0);
}

void PwmDriver::setOff(uint8_t pinNum)
{
    setPwm(pinNum, 0, 4096);
}

void PwmDriver::setEnableOutput(bool state)
{
    // OE pin low to enable outputs
    setOePin(!state);
}

void PwmDriver::sendCommand(Register reg, Data data)
{
    uint8_t buff[] = { (uint8_t)reg, (uint8_t)data };
    HAL_I2C_Master_Transmit(&hi2c2, m_slaveAddress, buff, 2, HAL_MAX_DELAY);
}

void PwmDriver::sendCommand(Register reg, uint8_t data)
{
    uint8_t buff[] = { (uint8_t)reg, data };
    HAL_I2C_Master_Transmit(&hi2c2, m_slaveAddress, buff, 2, HAL_MAX_DELAY);
}

uint8_t PwmDriver::read(Register reg)
{
    if (HAL_I2C_Master_Transmit(&hi2c2, m_slaveAddress, (uint8_t*)&reg, 1, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    uint8_t buff;
    HAL_I2C_Master_Receive(&hi2c2, m_slaveAddress, &buff, 1, HAL_MAX_DELAY);
    return buff;
}

void PwmDriver::reset()
{
    sendCommand(Register::MODE1, Data::MODE1_RESTART);
    HAL_Delay(10);
}

void PwmDriver::setPwmFrequency(float frequency)
{
    const uint8_t PRESCALE_MIN = 3; /**< minimum prescale value */
    const uint8_t PRESCALE_MAX = 255; /**< maximum prescale value */
    const uint32_t FREQUENCY_OSCILLATOR = 25000000; /**< Int. osc. frequency in datasheet */

    float prescaleval = ((FREQUENCY_OSCILLATOR / (frequency * 4096.0)) + 0.5) - 1;
    if (prescaleval < PRESCALE_MIN)
        prescaleval = PRESCALE_MIN;
    if (prescaleval > PRESCALE_MAX)
        prescaleval = PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    uint8_t oldmode = read(Register::MODE1);
    uint8_t newmode = (oldmode & ~(uint8_t)Data::MODE1_RESTART) | (uint8_t)Data::MODE1_SLEEP; // sleep
    sendCommand(Register::MODE1, newmode); // go to sleep
    sendCommand(Register::PRESCALE, prescale); // set the prescaler
    sendCommand(Register::MODE1, oldmode);
    HAL_Delay(5);
    // This sets the MODE1 register to turn on auto increment.
    sendCommand(Register::MODE1, oldmode | (uint8_t)Data::MODE1_RESTART | (uint8_t)Data::MODE1_AI);
}

void PwmDriver::setOutputMode(bool totempole)
{
    uint8_t oldmode = read(Register::MODE2);
    uint8_t newmode;
    if (totempole)
        newmode = oldmode | (uint8_t)Data::MODE2_OUTDRV;
    else
        newmode = oldmode & ~(uint8_t)Data::MODE2_OUTDRV;
    sendCommand(Register::MODE2, newmode);

    read(Register::MODE2);
}

void PwmDriver::setOePin(bool state)
{
    HAL_GPIO_WritePin(Pwm_Ctr_Enable_GPIO_Port, Pwm_Ctr_Enable_Pin, (GPIO_PinState)state);
}