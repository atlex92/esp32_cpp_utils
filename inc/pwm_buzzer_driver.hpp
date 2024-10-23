#pragma once

#include "driver/ledc.h"
#include "i_buzzer_driver.hpp"

class PwmBuzzerDriver : public IBuzzerDriver {
public:
  explicit PwmBuzzerDriver(const gpio_num_t gpio, const ledc_channel_t channel, const ledc_timer_t ledc_timer)
  : gpio_(gpio), ledc_channel_{channel}, ledc_timer_{ledc_timer} {
  }

  void initHardware() override {
    const ledc_timer_config_t ledc_timer_cfg{.speed_mode = LEDC_LOW_SPEED_MODE,
                                             .duty_resolution = LEDC_TIMER_10_BIT,
                                             .timer_num = ledc_timer_,
                                             .freq_hz = 100,
                                             .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_cfg));

    const ledc_channel_config_t ledc_channel_cfg{
        gpio_, LEDC_LOW_SPEED_MODE, ledc_channel_, LEDC_INTR_DISABLE, ledc_timer_, 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel_, kDuty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channel_));
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, ledc_channel_, 0));
  }

  virtual void setFrequency(const uint32_t freq) override {
    ESP_ERROR_CHECK(ledc_set_freq(LEDC_LOW_SPEED_MODE, ledc_timer_, freq));
  }

  void on() override {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel_, kDuty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channel_));
  }

  void off() override {
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, ledc_channel_, 0));
  }

private:
  static constexpr uint32_t kDuty{256U};
  const gpio_num_t gpio_{};
  const ledc_channel_t ledc_channel_{};
  const ledc_timer_t ledc_timer_{};
};