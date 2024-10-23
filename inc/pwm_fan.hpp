#pragma once

#include "driver/ledc.h"
#include "esp_err.h"
#include "hal/gpio_types.h"

struct PwmFanCfg {
  gpio_num_t cntrl_gpio;
  ledc_timer_t ledc_timer;
  ledc_channel_t ledc_channel;
};

class PwmFan {
public:
  explicit PwmFan(const PwmFanCfg& cfg) : cfg_{cfg} {
    initialize();
  }

  uint8_t speedPercent() const {
    return current_speed_percent_;
  }

  void setSpeedPercent(const uint8_t new_percent) {
    const auto duty{0xFF * new_percent / 100.0F};

    ESP_ERROR_CHECK(ledc_set_freq(LEDC_LOW_SPEED_MODE, cfg_.ledc_timer, kPwmFrequency));
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, cfg_.ledc_channel, duty));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, cfg_.ledc_channel));
    current_speed_percent_ = new_percent;
  }

  void off() {
    setSpeedPercent(0);
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, cfg_.ledc_channel, 0U));
  }

private:
  void initialize() {
    initLedTimer();
    initLedChannel();
    off();
  }

  void initLedTimer() {
    const ledc_timer_config_t ledc_timer_cfg{.speed_mode = LEDC_LOW_SPEED_MODE,
                                             .duty_resolution = LEDC_TIMER_8_BIT,
                                             .timer_num = cfg_.ledc_timer,
                                             .freq_hz = kPwmFrequency,
                                             .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_cfg));
  }

  void initLedChannel() {
    const ledc_channel_config_t ledc_channel_cfg{.gpio_num = cfg_.cntrl_gpio,
                                                 .speed_mode = LEDC_LOW_SPEED_MODE,
                                                 .channel = cfg_.ledc_channel,
                                                 .intr_type = LEDC_INTR_DISABLE,
                                                 .timer_sel = cfg_.ledc_timer,
                                                 .duty = 0,
                                                 .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
  }

  static constexpr uint16_t kPwmFrequency{30000U};
  PwmFanCfg cfg_{};
  uint8_t current_speed_percent_{};
};
