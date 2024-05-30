#pragma once

#include "abstract_button.hpp"
#include "driver/gpio.h"

enum eGpioButtonPressedLogicLevels {
    BUTTON_PRESSED_LEVEL_LOW = 0,
    BUTTON_PRESSED_LEVEL_HIGH = 1
};

class GpioButton : public AbstractButton {
    public:
        explicit GpioButton (const gpio_num_t gpio, const eGpioButtonPressedLogicLevels pressed_logic_level, const bool pullup, const uint32_t debounce_period_ms = 200U);
    private:
        bool initProcedure() override final;
        eButtonStates readState() const override final;
    private:
        const gpio_num_t gpio_{};
        const eGpioButtonPressedLogicLevels pressed_logic_level_{};
        const bool pullup_{};
        static constexpr const char* const TAG {"GpioButton"};
};