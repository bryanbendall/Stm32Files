#pragma once

#include "BinaryDeserializer.h"

class FramDeserializer : public Brytec::BinaryDeserializer {

public:
    FramDeserializer();

protected:
    virtual bool readInternal(uint8_t* data, uint32_t dataSize) override;
};