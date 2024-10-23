#pragma once

#include "stdint.h"

namespace TimeUtils {
bool isPeriodPassedMs(const uint32_t start, const uint32_t period);
bool isPeriodPassedMicros(const uint64_t start, const uint64_t period);
uint32_t nowMs();
uint64_t nowMicros();
}; // namespace TimeUtils