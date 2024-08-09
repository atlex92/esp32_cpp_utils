#pragma once

#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include <vector>
#include "initializable.h"

#pragma pack(push, 1)
struct RgbColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
#pragma pack(pop)

enum eColorFormats {
    COLOR_RGB,
    COLOR_GRB,
};

constexpr RgbColor makeColor(const uint8_t red, const uint8_t green, const uint8_t blue) {
    return RgbColor{red, green, blue};
}

class AdrressRgbController : public Initializable {
public:
    explicit AdrressRgbController(const gpio_num_t gpio, const uint32_t resolution_hz, const eColorFormats format = COLOR_RGB);
    
    bool setColor(const RgbColor& color, const size_t led_num) {
        return setColors(&color, 1U);
    }

    bool setColors(const std::vector<RgbColor>& colors) {
        return setColors(colors.data(), colors.size());
    }

    bool setColors(const RgbColor* colors, const size_t len) {
        return (ESP_OK == rmtTransmit(reinterpret_cast<const uint8_t*>(colors), sizeof(RgbColor) * len));
    }

    bool off(const size_t led) {
        RgbColor color{kOffColor};
        return setColor(color, led);
    }

    bool initializeImpl() override;

private:
    esp_err_t rmtTransmit(const uint8_t* data, const size_t len);
    static constexpr const char* const TAG{"AdrressRgbController"};
    static constexpr size_t kDefaultMemblockSymbols{64U};
    static constexpr size_t kDefaultTransQueueDepth{4U};
    static constexpr RgbColor kOffColor{makeColor(0, 0, 0)};
    const gpio_num_t gpio_{};
    const uint32_t resolution_{};
    const eColorFormats color_format_{};
    rmt_encoder_handle_t led_encoder_{};
    rmt_channel_handle_t led_chan_{};
};