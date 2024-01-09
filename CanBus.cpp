#include "CanBus.h"

#include "CanBusDefs.h"
#include "EBrytecApp.h"
#include "fdcan.h"
#include "stm32g4xx_hal_fdcan.h"

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
    Brytec::CanFrame frame;

    FDCAN_RxHeaderTypeDef header = {};
    /* Retrieve Rx messages from RX FIFO0 */
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, frame.data);
    frame.id = header.Identifier;
    if (header.IdType == FDCAN_EXTENDED_ID)
        frame.type = Brytec::CanFrameType::Ext;
    else
        frame.type = Brytec::CanFrameType::Std;
    frame.dlc = header.DataLength;

    uint8_t index = CanBusDefs::getCanBusIndex(hfdcan);
    if (index != UINT8_MAX)
        Brytec::EBrytecApp::canReceived(index, frame);
}

void CanBus::start(uint8_t index, Brytec::CanSpeed::Types speed)
{
    FDCAN_HandleTypeDef* canHandle = CanBusDefs::getCanBus(index);
    if (!canHandle)
        return;

    // Stop can to reconfigure
    if (HAL_FDCAN_DeInit(canHandle) != HAL_OK)
        return;

    // Can clock is 48 mhz
    // Calculations from : http://www.bittiming.can-wiki.info/
    // For this application seg1 and seg2 are always the same so just need to change the prescaler
    canHandle->Init.NominalTimeSeg1 = 13;
    canHandle->Init.NominalTimeSeg2 = 2;
    canHandle->Init.DataTimeSeg1 = 13;
    canHandle->Init.DataTimeSeg2 = 2;

    switch (speed) {
    case Brytec::CanSpeed::Types::Speed_1MBps:
        canHandle->Init.NominalPrescaler = 3;
        canHandle->Init.DataPrescaler = 3;
        break;
    case Brytec::CanSpeed::Types::Speed_500kBps:
        canHandle->Init.NominalPrescaler = 6;
        canHandle->Init.DataPrescaler = 6;
        break;
    case Brytec::CanSpeed::Types::Speed_250kBps:
        canHandle->Init.NominalPrescaler = 12;
        canHandle->Init.DataPrescaler = 12;
        break;
    case Brytec::CanSpeed::Types::Speed_200kBps:
        canHandle->Init.NominalPrescaler = 15;
        canHandle->Init.DataPrescaler = 15;
        break;
    case Brytec::CanSpeed::Types::Speed_125kBps:
        canHandle->Init.NominalPrescaler = 24;
        canHandle->Init.DataPrescaler = 24;
        break;
    case Brytec::CanSpeed::Types::Speed_100kBps:
        canHandle->Init.NominalPrescaler = 30;
        canHandle->Init.DataPrescaler = 30;
        break;
    case Brytec::CanSpeed::Types::Speed_50kBps:
        canHandle->Init.NominalPrescaler = 60;
        canHandle->Init.DataPrescaler = 60;
        break;
    case Brytec::CanSpeed::Types::Speed_20kBps:
        canHandle->Init.NominalPrescaler = 150;
        canHandle->Init.DataPrescaler = 150;
        break;

    default:
        return;
    }

    // Start can
    if (HAL_FDCAN_Init(canHandle) != HAL_OK)
        return;

    HAL_FDCAN_Start(canHandle);
    HAL_FDCAN_ActivateNotification(canHandle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

void CanBus::send(uint8_t index, const Brytec::CanFrame& frame)
{
    FDCAN_HandleTypeDef* canHandle = CanBusDefs::getCanBus(index);
    if (!canHandle)
        return;

    FDCAN_TxHeaderTypeDef header = {};
    header.Identifier = frame.id;
    if (frame.type == Brytec::CanFrameType::Ext)
        header.IdType = FDCAN_EXTENDED_ID;
    else
        header.IdType = FDCAN_STANDARD_ID;
    header.TxFrameType = FDCAN_FRAME_CLASSIC;
    header.DataLength = FDCAN_DLC_BYTES_8;
    header.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    header.BitRateSwitch = FDCAN_BRS_OFF;
    header.FDFormat = FDCAN_FRAME_CLASSIC;
    header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    header.MessageMarker = 0;

    HAL_FDCAN_AddMessageToTxFifoQ(canHandle, &header, (uint8_t*)frame.data);
}