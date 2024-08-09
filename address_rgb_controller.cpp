/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "address_rgb_controller.hpp"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"

typedef struct {
    uint32_t resolution; /*!< Encoder resolution, in Hz */
} led_strip_encoder_config_t;

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} rmt_led_strip_encoder_t;

static size_t rmt_encode_led_strip(rmt_encoder_t* encoder, rmt_channel_handle_t channel, const void* primary_data, size_t data_size, rmt_encode_state_t* ret_state) {
    
    rmt_led_strip_encoder_t* led_encoder{__containerof(encoder, rmt_led_strip_encoder_t, base)};
    rmt_encoder_handle_t bytes_encoder{led_encoder->bytes_encoder};
    rmt_encoder_handle_t copy_encoder{led_encoder->copy_encoder};
    rmt_encode_state_t session_state{RMT_ENCODING_RESET};
    uint16_t state{RMT_ENCODING_RESET};
    size_t encoded_symbols{};

    switch (led_encoder->state) {
        case 0: // send RGB data
            encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, primary_data, data_size, &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                led_encoder->state = 1; // switch to next state when current encoding session finished
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                break; // yield if there's no free space for encoding artifacts
            }
        // fall-through
        case 1: // send reset code
            encoded_symbols += copy_encoder->encode(copy_encoder, channel, &led_encoder->reset_code,
                                                    sizeof(led_encoder->reset_code), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                led_encoder->state = RMT_ENCODING_RESET; // back to the initial encoding session
                state |= RMT_ENCODING_COMPLETE;
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                break; // yield if there's no free space for encoding artifacts
            }
    }
    *ret_state = static_cast<rmt_encode_state_t>(state);
    return encoded_symbols;
}

static esp_err_t rmt_del_led_strip_encoder(rmt_encoder_t* encoder) {
    rmt_led_strip_encoder_t* led_encoder{__containerof(encoder, rmt_led_strip_encoder_t, base)};
    rmt_del_encoder(led_encoder->bytes_encoder);
    rmt_del_encoder(led_encoder->copy_encoder);
    free(led_encoder);
    return ESP_OK;
}

static esp_err_t rmt_led_strip_encoder_reset(rmt_encoder_t* encoder) {
    rmt_led_strip_encoder_t* led_encoder{__containerof(encoder, rmt_led_strip_encoder_t, base)};
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

esp_err_t rmt_new_led_strip_encoder(const led_strip_encoder_config_t* config, rmt_encoder_handle_t* ret_encoder) {
    esp_err_t ret{ESP_OK};
    rmt_led_strip_encoder_t* led_encoder{};
    assert(config && ret_encoder);
    led_encoder = static_cast<rmt_led_strip_encoder_t*>(rmt_alloc_encoder_mem(sizeof(rmt_led_strip_encoder_t)));
    assert(led_encoder);
    led_encoder->base.encode = rmt_encode_led_strip;
    led_encoder->base.del = rmt_del_led_strip_encoder;
    led_encoder->base.reset = rmt_led_strip_encoder_reset;
    // different led strip might have its own timing requirements, following parameter is for WS2812
    rmt_bytes_encoder_config_t bytes_encoder_config {
        .bit0 = {
            static_cast<uint16_t>(0.3 * config->resolution / 10'00'000), // T0H=0.3us
            1,
            static_cast<uint16_t>(0.9 * config->resolution / 10'00'000), // T0L=0.9us
            0,
        },
        .bit1 = {
            static_cast<uint16_t>(0.9 * config->resolution / 10'000'00), // T1H=0.9us
            1,
            static_cast<uint16_t>(0.3 * config->resolution / 10'00'000), // T1L=0.3us
            0,
        },
        .flags = {
            .msb_first = 1
        } // WS2812 transfer bit order: G7...G0R7...R0B7...B0
    };
    esp_err_t err{};
    err = rmt_new_bytes_encoder(&bytes_encoder_config, &led_encoder->bytes_encoder);
    rmt_copy_encoder_config_t copy_encoder_config = {};
    if(ESP_OK == err) {
        err = rmt_new_copy_encoder(&copy_encoder_config, &led_encoder->copy_encoder);
    }

    if(ESP_OK == err) {
        uint32_t reset_ticks{config->resolution / 1000000 * 50 / 2}; // reset code duration defaults to 50us
        led_encoder->reset_code = rmt_symbol_word_t {
            0,
            static_cast<uint16_t>(reset_ticks),
            0,
            static_cast<uint16_t>(reset_ticks),
        };
        *ret_encoder = &led_encoder->base;
    } else {
        if (led_encoder) {
            if (led_encoder->bytes_encoder) {
                rmt_del_encoder(led_encoder->bytes_encoder);
            }
            if (led_encoder->copy_encoder) {
                rmt_del_encoder(led_encoder->copy_encoder);
            }
            free(led_encoder);
        }
    }
    return ret;
}

AdrressRgbController::AdrressRgbController(const gpio_num_t gpio, const uint32_t resolution_hz, const eColorFormats format)
    :   gpio_{gpio}, resolution_{resolution_hz}, color_format_{format} {

}

esp_err_t AdrressRgbController::rmtTransmit(const uint8_t* data, const size_t len) {
    rmt_transmit_config_t tx_config {
        .loop_count = 0, // no transfer loop
    };

    const uint8_t* data_to_send{};
    std::vector<uint8_t> transformed_data;
    if(eColorFormats::COLOR_GRB == color_format_) {
        transformed_data.resize(len);
        for (size_t i = 0; i < len; i += 3) {
            transformed_data[i] = data[i + 1];
            transformed_data[i + 1] = data[i];
            transformed_data[i + 2] = data[i + 2];
        }
        data_to_send = transformed_data.data();
    } else {
        data_to_send = data;
    }
    esp_err_t err{rmt_transmit(led_chan_, led_encoder_, data_to_send, len, &tx_config)};

    if(ESP_OK == err) {
        err = rmt_tx_wait_all_done(led_chan_, portMAX_DELAY);
    }

    return err;
}

bool AdrressRgbController::initializeImpl() {
    bool ret{};
    rmt_tx_channel_config_t tx_chan_config  {
        .gpio_num = gpio_,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .resolution_hz = resolution_,
        .mem_block_symbols = kDefaultMemblockSymbols, // increase the block size can make the LED less flickering
        .trans_queue_depth = kDefaultTransQueueDepth, // set the number of transactions that can be pending in the background
    };
    esp_err_t err{rmt_new_tx_channel(&tx_chan_config, &led_chan_)};

    if(ESP_OK == err) {
        led_strip_encoder_config_t encoder_config {
            .resolution = resolution_,
        };
        err = rmt_new_led_strip_encoder(&encoder_config, &led_encoder_);
    }

    if(ESP_OK == err) {
        err = rmt_enable(led_chan_);
        if(ESP_OK == err) {
            ret = true;
        }
    }

    return ret;
}
