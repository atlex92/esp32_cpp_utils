#include "adc_channel.hpp"

adc_oneshot_unit_handle_t AdcChannel::adc_handle_{nullptr};
Mutex AdcChannel::mutex_{};