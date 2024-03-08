#pragma once

#include <functional>
#include "driver/gptimer.h"

class Timer {
public:
    using onAlarmCallbackType = std::function<void()>; 
    explicit Timer(const gptimer_count_direction_t timer_direction, const uint32_t resolution) {
        const gptimer_config_t cfg {
            GPTIMER_CLK_SRC_DEFAULT,
            timer_direction,
            resolution,
            0,
            {}
        };

        const gptimer_event_callbacks_t callbacks_cfg{
            alarmCallback
        };
        ESP_ERROR_CHECK(gptimer_new_timer(&cfg, &handle_));
        ESP_ERROR_CHECK(gptimer_register_event_callbacks(handle_, &callbacks_cfg, this));
        ESP_ERROR_CHECK(gptimer_enable(handle_));
        setAlarmValue(resolution);
    }

    ~Timer() {
        ESP_ERROR_CHECK(gptimer_del_timer(handle_));
    }

    void start() {
        ESP_ERROR_CHECK(gptimer_start(handle_));
    }

    void stop() {
        gptimer_stop(handle_);
    }

    uint64_t value() const {
        uint64_t ret{};
        ESP_ERROR_CHECK(gptimer_get_raw_count(handle_, &ret));
        return ret;
    }

    void setValue(const uint64_t value) {
        ESP_ERROR_CHECK(gptimer_set_raw_count(handle_, value));
    }

    void setAutoReload(const bool reload) {
        alarm_cfg_.flags.auto_reload_on_alarm = reload;
        ESP_ERROR_CHECK(gptimer_set_alarm_action(handle_, &alarm_cfg_));
    }

    void setAlarmValue(const uint64_t value) {
        alarm_cfg_.alarm_count = value;
        ESP_ERROR_CHECK(gptimer_set_alarm_action(handle_, &alarm_cfg_));
    }

    void setReloadValue(const uint64_t value) {
        alarm_cfg_.reload_count = value;
        ESP_ERROR_CHECK(gptimer_set_alarm_action(handle_, &alarm_cfg_));
    }

    void onAlarm(onAlarmCallbackType cb) {
        on_alarm_cb_ = cb;
    }

    static bool alarmCallback(gptimer_handle_t timer, const gptimer_alarm_event_data_t* data, void* user_ctx) {
        Timer* timer_obj{static_cast<Timer*>(user_ctx)};
        if(timer_obj->on_alarm_cb_) {
            timer_obj->on_alarm_cb_();
        }
        return true;
    }
private:
    gptimer_handle_t handle_{};
    gptimer_alarm_config_t alarm_cfg_{};
    onAlarmCallbackType on_alarm_cb_{};
};
