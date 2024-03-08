#pragma once

#include "stdint.h"
#include "math.h"

struct NtcCfg {
    float nominal_r;
    float nominal_t;
    float b_coeff;
    bool is_upper;
    float second_resistance;
    float vcc;
};
class NtcSensor {

public:
    NtcSensor(const NtcCfg& cfg)
        :   cfg_{cfg} {
    }

    float temperature(const float voltage) const {
        return ntcResistanceToTemperatue(resistance(voltage), cfg_.nominal_r, cfg_.b_coeff, cfg_.nominal_t);
    }

    float resistance(const float voltage) const {
        return getResistanceInDivider(voltage);
    }

private:
    static constexpr const char* const TAG{"NtcResistanceToTemperature"};

    /*
        Simplified Steinhart-Hart equation
        1/T = 1/T0 + 1/B * ln(R/R0)
    */
    static float ntcResistanceToTemperatue(const uint32_t resistance, const uint32_t nominal_resistance, const uint32_t b_coeff, const float nominal_temp) {

        if (0 == resistance){
            ESP_LOGE(TAG, "resistance is 0");
            return KInvalidTemperature;
        }
        if (0 == b_coeff) {
            ESP_LOGE(TAG, "b_coeff is 0");
            return KInvalidTemperature;
        }
        if(0 == nominal_resistance) {
            ESP_LOGE(TAG, "nominal_resistance is 0");
            return KInvalidTemperature;
        }

        float ret{static_cast<float>(resistance)/nominal_resistance};
        ret = log(ret);
        ret /= b_coeff;
        ret += 1.0F/(nominal_temp + kCelciusOffset);
        ret = 1.0F / ret;
        ret -= kCelciusOffset;
        return ret;
    }

    float getResistanceInDivider(const float voltage) const {
        float ret{};
        if(cfg_.is_upper) {
            ret = cfg_.second_resistance * (cfg_.vcc/voltage - 1.0F);
        } else {
            ret = cfg_.second_resistance / (cfg_.vcc/voltage - 1.0F);
        }
        return ret;
    }

    NtcCfg cfg_{};
    static constexpr float kCelciusOffset{273.15F};
    static constexpr int KInvalidTemperature{0x7FFFFFFF};
};