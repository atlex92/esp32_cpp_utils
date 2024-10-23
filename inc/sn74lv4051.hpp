#pragma once

#include "driver/gpio.h"

enum eSn74lv4051MuxAddresses {
  Y0 = 0,
  Y1,
  Y2,
  Y3,
  Y4,
  Y5,
  Y6,
  Y7,
};

class Sn74lv4051 {
public:
  explicit Sn74lv4051(const gpio_num_t a, const gpio_num_t b, const gpio_num_t c, const gpio_num_t en)
  : gpio_a_{a}, gpio_b_{b}, gpio_c_{c}, gpio_en_{en} {
    ESP_ERROR_CHECK(gpio_set_direction(gpio_a_, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(gpio_b_, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(gpio_c_, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(gpio_en_, GPIO_MODE_OUTPUT));
    disable();
  }

  void enable() {
    gpio_set_level(gpio_en_, 0);
  }

  void disable() {
    gpio_set_level(gpio_en_, 1);
  }

  void setAddress(const eSn74lv4051MuxAddresses address) {
    gpio_set_level(gpio_a_, (address & 1) ? 1 : 0);
    gpio_set_level(gpio_b_, ((address >> 1) & 1) ? 1 : 0);
    gpio_set_level(gpio_c_, ((address >> 2) & 1) ? 1 : 0);
  }

private:
  const gpio_num_t gpio_a_{};
  const gpio_num_t gpio_b_{};
  const gpio_num_t gpio_c_{};
  const gpio_num_t gpio_en_{};
};