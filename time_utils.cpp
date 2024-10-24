#include "time_utils.hpp"
#include <time.h>
#include <unistd.h>
#include "esp32_utils.hpp"

bool TimeUtils::isPeriodPassedMs(const uint32_t start, const uint32_t period) {
  return TimeUtils::nowMs() >= (start + period);
}

bool TimeUtils::isPeriodPassedMicros(const uint64_t start, const uint64_t period) {
  return TimeUtils::nowMicros() >= (start + period);
}

uint32_t TimeUtils::nowMs() {
#ifndef NATIVE_BUILD
  return ESP32Utils::millis();
#else
  return 1000UL * (float) clock() / CLOCKS_PER_SEC;
#endif
}

uint64_t TimeUtils::nowMicros() {
  return ESP32Utils::micros();
}