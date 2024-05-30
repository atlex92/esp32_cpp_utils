#include "gpio_button.hpp"
#include "esp_log.h"
#include "gpio_utils.hpp"

static const char* const TAG {"GpioButton"};

GpioButton::GpioButton(const gpio_num_t gpio, const eGpioButtonPressedLogicLevels pressed_logic_level, const bool pullup, const uint32_t debounce_period_ms)
    :   AbstractButton(debounce_period_ms),
        gpio_(gpio),
        pressed_logic_level_(pressed_logic_level),
        pullup_(pullup) {
    
    if(GPIO_NUM_MAX <= gpio) {
        ESP_LOGE(TAG, "invalid pinNum: %u", gpio);
    }
}

bool GpioButton::initProcedure() {
    // if (ESP_OK != GpioUtils::resetGpio(gpio_)) {
    //     ESP_LOGE(TAG, "Failed to reset gpio %u!", gpio_);
    //     return false;
    // }
    
    if (ESP_OK != GpioUtils::setGpioMode(gpio_, gpio_mode_t::GPIO_MODE_INPUT, pullup_, false)) {
        ESP_LOGE(TAG, "Failed to set INPUT mode to gpio %u!", gpio_);
        return false;
    }
    return true;
}

eButtonStates GpioButton::readState() const {
    const GpioUtils::eGpioOutputValues gpio_level {GpioUtils::gpioRead(gpio_)};
    if (eGpioButtonPressedLogicLevels::BUTTON_PRESSED_LEVEL_HIGH == pressed_logic_level_) {
        return GpioUtils::eGpioOutputValues::GPIO_HIGH == gpio_level ? eButtonStates::BUTTON_PRESSED : eButtonStates::BUTTON_RELEASED;
    }
    else {
        return GpioUtils::eGpioOutputValues::GPIO_HIGH == gpio_level ? eButtonStates::BUTTON_RELEASED : eButtonStates::BUTTON_PRESSED;
    }
}