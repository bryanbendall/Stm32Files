#include "FramDeserializer.h"

#include "Fram.h"

FramDeserializer::FramDeserializer()
{
    m_currentOffset = 2;
}

bool FramDeserializer::readInternal(uint8_t* data, uint32_t dataSize)
{
    if (m_currentOffset + dataSize > Fram::maxSize)
        return false;

    bool ret = Fram::read(m_currentOffset, data, dataSize);
    m_currentOffset += dataSize;
    return ret;
}