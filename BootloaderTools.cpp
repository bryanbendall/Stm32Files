#include "BootloaderTools.h"

#if __has_include("stm32g4xx_hal.h")
#include "stm32g4xx_hal.h"
#define conBootloadAddress 0x1FFF0000
#define magicDataAddress 0x2001B000
#endif

#define magicData 0xDEADBEEF

void jumpToBootloader()
{
    void (*SysMemBootJump)(void);
    uint8_t i;

    // Reset clock to default
    HAL_RCC_DeInit();

    // Disable Systick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // Clear all interrupt bits
    for (i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    SysMemBootJump = (void (*)(void))(*((uint32_t*)(conBootloadAddress + 4)));
    __set_MSP(*(uint32_t*)conBootloadAddress);
    SysMemBootJump();

    while (1)
        ; // Just in case...
}

void checkAndGoToBootloader()
{
    if (*((unsigned long*)magicDataAddress) == magicData) {
        *((unsigned long*)magicDataAddress) = 0;
        jumpToBootloader();
    }
}

void setShouldGoToBootloader()
{
    *((unsigned long*)magicDataAddress) = magicData;
    NVIC_SystemReset();
}
