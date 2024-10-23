#pragma once

#include <functional>
#include "driver/gptimer.h"
#include "initializable.h"

class Timer : public Initializable {
public:
  using onAlarmCallbackType = std::function<void()>;
  explicit Timer(const gptimer_count_direction_t timer_direction, const uint32_t resolution) {
    cfg_ = {GPTIMER_CLK_SRC_DEFAULT, timer_direction, resolution, 0, {}};
  }

  ~Timer() {
    ESP_ERROR_CHECK(gptimer_del_timer(handle_));
  }

  void start() {
    ESP_ERROR_CHECK(gptimer_start(handle_));
    is_started_ = true;
  }

  void stop() {
    gptimer_stop(handle_);
    is_started_ = false;
  }

  bool isStarted() const {
    return is_started_;
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
    if (timer_obj->on_alarm_cb_) {
      timer_obj->on_alarm_cb_();
    }
    return true;
  }

private:
  bool initializeImpl() override {
    const gptimer_event_callbacks_t callbacks_cfg{alarmCallback};
    ESP_ERROR_CHECK(gptimer_new_timer(&cfg_, &handle_));
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(handle_, &callbacks_cfg, this));
    ESP_ERROR_CHECK(gptimer_enable(handle_));
    setAlarmValue(cfg_.resolution_hz);

    return true;
  }
  gptimer_handle_t handle_{};
  gptimer_alarm_config_t alarm_cfg_{};
  onAlarmCallbackType on_alarm_cb_{};
  bool is_started_{false};
  gptimer_config_t cfg_{};
};
