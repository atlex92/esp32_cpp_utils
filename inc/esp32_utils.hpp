#pragma once

#include <stdint.h>
#include "esp_system.h"

namespace ESP32Utils {
void reboot();
uint32_t chipID();
esp_reset_reason_t rebootReason();
uint64_t millis();
uint64_t micros();
void delayMicros(const uint64_t micros);
void delayMs(const uint64_t ms);
}; // namespace ESP32Utils