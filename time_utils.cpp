#include "time_utils.hpp"
#include "esp32_utils.hpp"
#include <unistd.h>
#include <time.h>

bool TimeUtils::isPeriodPassed(const uint32_t start, const uint32_t period) {
    return TimeUtils::nowMs() >= (start + period);
}

uint32_t TimeUtils::nowMs() {
    #ifndef NATIVE_BUILD
        return ESP32Utils::millis();
    #else
        return 1000UL * (float)clock()/CLOCKS_PER_SEC;
    #endif
}