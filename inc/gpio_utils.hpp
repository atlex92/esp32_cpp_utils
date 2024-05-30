#pragma once

#include "driver/gpio.h"

namespace GpioUtils {
    enum eGpioOutputValues {
        GPIO_LOW = 0,
        GPIO_HIGH = 1
    };
    esp_err_t setGpioMode(const gpio_num_t gpio, const gpio_mode_t mode, const bool pullup, const bool pulldown);
    esp_err_t setGpioModeForGroup(const uint64_t gpio_bitmask, const gpio_mode_t mode, const bool pullup, const bool pulldown);
    esp_err_t gpioWrite(const gpio_num_t gpio, const eGpioOutputValues value);
    esp_err_t gpioSetHigh(const gpio_num_t gpio);
    esp_err_t gpioSetLow(const gpio_num_t gpio);
    esp_err_t resetGpio(const gpio_num_t gpio);
    esp_err_t disablePullDown(const gpio_num_t gpio);
    esp_err_t enablePullDown(const gpio_num_t gpio);
    eGpioOutputValues gpioRead(const gpio_num_t gpio);
};