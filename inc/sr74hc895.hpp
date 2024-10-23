#pragma once

#include <vector>
#include "driver/gpio.h"
#include "esp32_utils.hpp"

class Sr74hc595 {
public:
  explicit Sr74hc595(const gpio_num_t clk_gpio, const gpio_num_t data_gpio, const gpio_num_t latch_gpio)
  : clk_gpio_{clk_gpio}, data_gpio_{data_gpio}, latch_gpio_{latch_gpio}, have_reset_and_oe_{false} {
    gpio_reset_pin(clk_gpio_);
    gpio_reset_pin(data_gpio_);
    gpio_reset_pin(latch_gpio_);
    gpio_set_direction(clk_gpio_, GPIO_MODE_OUTPUT);
    gpio_set_direction(data_gpio_, GPIO_MODE_OUTPUT);
    gpio_set_direction(latch_gpio_, GPIO_MODE_OUTPUT);
    lockStorageRegister();
  }

  explicit Sr74hc595(const gpio_num_t clk_gpio, const gpio_num_t data_gpio, const gpio_num_t latch_gpio,
                     const gpio_num_t oe_gpio, const gpio_num_t reset_gpio)
  : Sr74hc595{clk_gpio, data_gpio, latch_gpio} {
    oe_gpio_ = oe_gpio;
    reset_gpio_ = reset_gpio;
    have_reset_and_oe_ = true;
    gpio_reset_pin(oe_gpio_);
    gpio_reset_pin(reset_gpio_);
    gpio_set_direction(oe_gpio_, GPIO_MODE_OUTPUT);
    gpio_set_direction(reset_gpio_, GPIO_MODE_OUTPUT);
    disableOutput();
    gpio_set_level(reset_gpio_, 1);
  }

  void write(const uint8_t data) {
    unlockStorageRegister();
    shiftOutData(data);
    lockStorageRegister();
  }

  void write(const std::vector<uint8_t>& data) {
    unlockStorageRegister();
    for (const auto byte : data) {
      shiftOutData(byte);
    }
    lockStorageRegister();
  }

  void enableOutput() {
    gpio_set_level(oe_gpio_, 0);
  }

  void disableOutput() {
    gpio_set_level(oe_gpio_, 1);
  }

  void resetData() {
    gpio_set_level(reset_gpio_, 0);
    ESP32Utils::delayMs(1);
    gpio_set_level(reset_gpio_, 1);
  }

private:
  void lockStorageRegister() {
    gpio_set_level(latch_gpio_, 1);
  }

  void unlockStorageRegister() {
    gpio_set_level(latch_gpio_, 0);
  }

  void shiftOutData(const uint8_t data) {
    for (size_t i{kBitsInByte}; i > 0; i--) {
      gpio_set_level(data_gpio_, (data >> (i - 1)) & 1U);
      clockData();
    }
  }

  void clockData() {
    gpio_set_level(clk_gpio_, 1);
    ESP32Utils::delayMicros(500);
    gpio_set_level(clk_gpio_, 0);
    ESP32Utils::delayMicros(500);
  }

  gpio_num_t clk_gpio_{};
  gpio_num_t data_gpio_{};
  gpio_num_t latch_gpio_{};
  gpio_num_t oe_gpio_{};
  gpio_num_t reset_gpio_{};
  bool have_reset_and_oe_{false};
  static constexpr size_t kBitsInByte{8U};
};