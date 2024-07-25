#pragma once

#include "i_buzzer_driver.hpp"
#include "timer.hpp"

class HwTimerBuzzerDriver : public IBuzzerDriver {
public:

    explicit HwTimerBuzzerDriver(const gpio_num_t gpio)
        :   gpio_(gpio) {
        ESP_ERROR_CHECK(gpio_set_direction(gpio_, GPIO_MODE_OUTPUT));
    }

    void initHardware() override {
        static auto onAlarmCb = [this]() {            
            ESP_ERROR_CHECK(gpio_set_level(this->gpio_, this->is_gpio_high_));
            this->is_gpio_high_ = !this->is_gpio_high_;
        };
        timer_.onAlarm(onAlarmCb);
        timer_.setAutoReload(true);
    }

    void on() override {
        if(!is_on_) {
            is_gpio_high_ = true;
            timer_.setAlarmValue(alarmValue_);
            timer_.start();
            is_on_ = true;
        }
    }

    void off() override {
        timer_.stop();
        is_on_ = false;
    }

    virtual void setFrequency(const uint32_t freq) override {
        alarmValue_ = kResolution / (freq * 2);
        timer_.setAlarmValue(alarmValue_);
    }

private:
    const gpio_num_t gpio_{};
    bool is_gpio_high_{};
    static constexpr uint64_t kResolution{10'000'000};
    bool is_on_{false};
    Timer timer_{GPTIMER_COUNT_UP, kResolution};
    uint64_t alarmValue_{};
};