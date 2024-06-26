#pragma once

#include "task.hpp"
#include "adc_channel.hpp"
#include "mutex.hpp"
#include "mutex_locker.hpp"
#include "moving_average_filter.hpp"
#include "fw_config.hpp"

template <unsigned int window>
class AdcAsyncChannel : public Task {
public:
    explicit AdcAsyncChannel(const uint32_t period_ms, const adc_unit_t unit, const adc_channel_t channel, const adc_bitwidth_t bitwidth, const adc_atten_t attenuation)
        :   Task{"AsyncAdcChannel", configMINIMAL_STACK_SIZE*2},
            adc_channel_{unit, channel, bitwidth, attenuation},
            period_ms_{period_ms} {
    }

    int getAdcValue() const {
        int ret{};
        {
            MutexLocker{mutex_};
            ret = ma_adc_filter_.filteredValue();
        }
        return ret;
    }

private:
    void run(void* data) override {
        assert(adc_channel_.initialize());
        delay(100);
        while(1) {
            {
                MutexLocker{mutex_};
                ma_adc_filter_.process(adc_channel_.read());
            }
            adc_channel_.cnangeAttenuation(adc_channel_.getTheBestAttenuation(ma_adc_filter_.filteredValue()));
            delay(period_ms_);
        }
    }
    mutable Mutex mutex_{};
    AdcChannel adc_channel_;
    MAFilter<float, window> ma_adc_filter_{};
    const uint32_t period_ms_{};
};