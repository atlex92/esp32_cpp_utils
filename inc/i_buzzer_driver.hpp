#pragma once

#include <stdint.h>

class IBuzzerDriver {
public:
    virtual ~IBuzzerDriver() = default;
    virtual void initHardware() = 0;
    virtual void setFrequency(const uint32_t freq) = 0;
    virtual void on() = 0;
    virtual void off() = 0;
};