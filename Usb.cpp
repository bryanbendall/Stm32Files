#include "Usb.h"

#include "BinaryBufferSerializer.h"
#include "BootloaderTools.h"
#include "CanBus.h"
#include "Deserializer/BinaryArrayDeserializer.h"
#include "EBrytecApp.h"
#include "Usb/ModuleCommand.h"
#include "Utils/RingBuffer.h"
#include "usbd_cdc_if.h"

static Brytec::RingBuffer<Brytec::UsbPacket, USB_BUFFER_SIZE> s_sendBuffer;

extern "C" int _write(int file, char* ptr, int len)
{
    if (len == 0)
        return -1;

    if (len > Brytec::UsbPacket::bufferSize - 1)
        len = Brytec::UsbPacket::bufferSize - 1;

    Brytec::UsbPacket packet;
    packet.data[0] = (uint8_t)Brytec::UsbPacketType::DebugPrint;
    memcpy(&packet.data[1], ptr, len);
    packet.length = len + 1;

    s_sendBuffer.add(packet);

    return len;
}

void Usb::send(Brytec::UsbPacket& packet)
{
    s_sendBuffer.add(packet);
}

extern USBD_HandleTypeDef hUsbDeviceFS;

void Usb::update()
{

    // Send
    {
        static uint8_t rawSend[64];
        USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
        // Test if the USB CDC is ready to transmit
        if (hcdc->TxState == 0 && s_sendBuffer.size() > 0) {
            Brytec::UsbPacket sendPacket = s_sendBuffer.get();
            rawSend[0] = Brytec::PacketStart;
            rawSend[1] = sendPacket.length;
            memcpy(&rawSend[2], sendPacket.data, sendPacket.length);
            USBD_CDC_SetTxBuffer(&hUsbDeviceFS, rawSend, sendPacket.length + 2);
            USBD_CDC_TransmitPacket(&hUsbDeviceFS);
        }
    }

    // Receive
    {
        static uint8_t rxBuffer[APP_RX_DATA_SIZE];

        uint32_t dataLength = vcp_recv(rxBuffer, APP_RX_DATA_SIZE);

        uint32_t i = 0;
        while (i < dataLength) {
            if (rxBuffer[i] == Brytec::PacketStart && (dataLength - i) > 3) {

                Brytec::UsbPacket packet;
                packet.length = rxBuffer[i + 1];
                if (packet.length <= 62 && (dataLength - i - packet.length) >= 0) {
                    memcpy(packet.data, &rxBuffer[i + 2], packet.length);
                    i += (2 + packet.length);
                    // Valid packet, do something with it

                    switch (packet.getType()) {
                    case Brytec::UsbPacketType::Command: {
                        Brytec::ModuleCommand moduleCommand = packet.as<Brytec::ModuleCommand>();
                        if (moduleCommand) {
                            switch (moduleCommand.command) {
                            case Brytec::ModuleCommand::Command::GoToBootloader:
                                setShouldGoToBootloader();
                                break;
                            }
                        }
                        break;
                    }
                    case Brytec::UsbPacketType::Can:
                        Brytec::EBrytecApp::brytecUsbReceived(packet);
                        break;
                    case Brytec::UsbPacketType::Status:
                        break;
                    case Brytec::UsbPacketType::Unknown:
                        break;
                    case Brytec::UsbPacketType::DebugPrint:
                        break;
                    }

                } else {
                    i++;
                }

            } else {
                i++;
            }
        }
    }
}