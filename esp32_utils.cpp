
#include "esp32_utils.hpp"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_timer.h"

void ESP32Utils::reboot() {
    esp_restart();
}

uint32_t ESP32Utils::chipID() {
    uint64_t _chipmacid = 0LL;
    esp_efuse_mac_get_default((uint8_t*) (&_chipmacid));
    const uint64_t res {_chipmacid>>24};
    return static_cast<uint32_t>(res);
}

esp_reset_reason_t ESP32Utils::rebootReason() {
    return esp_reset_reason();
}

uint32_t ESP32Utils::millis() {
    static const uint64_t msFactor {1000ULL};
    return static_cast<uint32_t>(esp_timer_get_time()/msFactor);
}

uint64_t ESP32Utils::micros() {
    return static_cast<uint64_t>(esp_timer_get_time());
}

void ESP32Utils::delayMicros(const uint64_t micros) {

    const uint64_t now{static_cast<uint64_t>(esp_timer_get_time())};

    if(micros > 0) {
        const uint64_t timestamp{now + micros};
        if(now > timestamp){ //overflow
            while(static_cast<uint64_t>(esp_timer_get_time()) > timestamp){
                asm volatile ("nop");
            }
        }
        while(static_cast<uint64_t>(esp_timer_get_time()) < timestamp){
            asm volatile ("nop");
        }
    }
}

void ESP32Utils::delayMs(const uint64_t ms) {
    delayMicros(ms * 1000U);
}