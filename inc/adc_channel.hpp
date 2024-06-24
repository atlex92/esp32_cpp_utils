#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "initializable.h"
#include "mutex_locker.hpp"

class AdcChannel : public Initializable {
public:
    explicit AdcChannel(const adc_unit_t unit, const adc_channel_t channel, const adc_bitwidth_t bitwidth, const adc_atten_t attenuation)
        :   unit_{unit},
            channel_{channel},
            bitwidth_{bitwidth},
            attenuation_{attenuation} {

    }

    void cnangeAttenuation(const adc_atten_t attenuation) {
        attenuation_ = attenuation;
        configure();
    }

    int read() const {
        int ret{kAdcError};
        int read_res{};
        const auto res{adc_oneshot_get_calibrated_result(adc_handle_, adc_cali_[attenuation_], channel_, &read_res)};
        if(ESP_OK == res) {
            ret = read_res;
        } else {
            ESP_LOGE(TAG, "adc error, code = %d", (int)res);
        }
        return ret;
    }

    static adc_atten_t getTheBestAttenuation(const int mvolts) {
        adc_atten_t ret{ADC_ATTEN_DB_0};
        if(mvolts >= kMaxVoltageDb6) {
            ret = ADC_ATTEN_DB_11;
        } else if(mvolts >= kMaxVoltageDb2_5) {
            ret = ADC_ATTEN_DB_6;
        } else if(mvolts > kMaxVoltageDb0) {
            ret = ADC_ATTEN_DB_2_5;
        }
        return ret;
    }

    static constexpr int kAdcError{-1};
private:
    bool initializeImpl() override {
        {
            MutexLocker locker{mutex_};
            if(!adc_handle_) {
                const adc_oneshot_unit_init_cfg_t adc_cfg{
                    unit_,
                    ADC_RTC_CLK_SRC_RC_FAST,
                    ADC_ULP_MODE_DISABLE
                };
                ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, &adc_handle_));
            }
        }

        const adc_atten_t attenuations[4] {
            ADC_ATTEN_DB_0,
            ADC_ATTEN_DB_2_5,
            ADC_ATTEN_DB_6,
            ADC_ATTEN_DB_11
        };

        adc_cali_curve_fitting_config_t cali_cfg{
            unit_,
            channel_,
            ADC_ATTEN_DB_0,
            bitwidth_
        };

        for (size_t i{}; i < 4; i++) {
            cali_cfg.atten = attenuations[i];
            ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_cfg, &adc_cali_[i]));
        }

        configure();

        return true;
    }

    void configure() {
        const adc_oneshot_chan_cfg_t channel_cfg {
            attenuation_,
            bitwidth_,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_, channel_, &channel_cfg));
    }

    adc_unit_t unit_{};
    adc_channel_t channel_{};
    adc_bitwidth_t bitwidth_{};
    adc_atten_t attenuation_{};
    static adc_oneshot_unit_handle_t adc_handle_;
    adc_cali_handle_t adc_cali_[4]{};
    static constexpr const char* const TAG{"AdcChannel"};
    static const int kMaxVoltageDb6{1600};
    static const int kMaxVoltageDb2_5{1150};
    static const int kMaxVoltageDb0{900};
    static Mutex mutex_;
};