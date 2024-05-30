#include "gpio_utils.hpp"

esp_err_t GpioUtils::setGpioMode(const gpio_num_t gpio, const gpio_mode_t mode, const bool pullup, const bool pulldown) {
    esp_err_t ret{gpio_set_direction(gpio, mode)};
    if(ESP_OK == ret) {
        ret = pullup ? gpio_pullup_en(gpio) : gpio_pullup_dis(gpio);
    }
    if(ESP_OK == ret) {
        ret = pulldown ? gpio_pulldown_en(gpio) : gpio_pulldown_dis(gpio);
    }
    return ret;
}

esp_err_t GpioUtils::setGpioModeForGroup(const uint64_t gpio_bitmask, const gpio_mode_t mode, const bool pullup, const bool pulldown) {
    const gpio_config_t conf {
        .pin_bit_mask = gpio_bitmask,
        .mode = mode,
        .pull_up_en = pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = gpio_int_type_t::GPIO_INTR_DISABLE
    };
    return gpio_config(&conf);
}

esp_err_t GpioUtils::disablePullDown(const gpio_num_t gpio) {
    return gpio_pulldown_dis(gpio);
}

esp_err_t GpioUtils::enablePullDown(const gpio_num_t gpio) {
    return gpio_pulldown_en(gpio);
}

esp_err_t GpioUtils::gpioWrite(const gpio_num_t gpio, const eGpioOutputValues value) {
    return gpio_set_level(gpio, static_cast<uint32_t>(value));
}

GpioUtils::eGpioOutputValues GpioUtils::gpioRead(const gpio_num_t gpio) {
    return gpio_get_level(gpio) == 1 ? eGpioOutputValues::GPIO_HIGH : eGpioOutputValues::GPIO_LOW;
}

esp_err_t GpioUtils::gpioSetHigh(const gpio_num_t gpio) {
    return gpioWrite(gpio, eGpioOutputValues::GPIO_HIGH);
}

esp_err_t GpioUtils::gpioSetLow(const gpio_num_t gpio) {
    return gpioWrite(gpio, eGpioOutputValues::GPIO_LOW);
}

esp_err_t GpioUtils::resetGpio(const gpio_num_t gpio) {
    return gpio_reset_pin(gpio);
}