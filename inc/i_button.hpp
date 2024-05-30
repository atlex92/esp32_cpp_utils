#pragma once

#include "stdint.h"
#include <functional>

enum eButtonStates {
    BUTTON_RELEASED = 0,
    BUTTON_PRESSED = 1,
    BUTTON_UNKNOWN = 2,
};

using ButtonCallback = std::function<void(void)>;
struct IButton {
    virtual ~IButton() = default;
    virtual eButtonStates buttonState() const = 0;
    virtual bool isPressed() const = 0;
    virtual bool isReleased() const = 0;
    virtual bool isHeldForMs(const uint32_t time_ms) const = 0;
    virtual bool isReleasedForMs(const uint32_t time_ms) const = 0;
    virtual void onPressed(ButtonCallback cb) = 0;
    virtual void onReleased(ButtonCallback cb) = 0;
    virtual void onHeldForMs(ButtonCallback cb, const uint32_t held_period_ms) = 0;
    virtual bool init() = 0;
};