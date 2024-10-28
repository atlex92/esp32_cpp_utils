#include "nvs.hpp"
#include <memory>
#include "esp_log.h"
#include "nvs.h"

std::optional<std::string> Nvs::read(const char* const tag) {

    std::optional<std::string> ret{std::nullopt};

    if (openToRead()) {
        size_t read_len{};
        esp_err_t err = nvs_get_str(nvs_handle_, tag, NULL, &read_len);
        std::string content{"", read_len};
        err = nvs_get_str(nvs_handle_, tag, content.data(), &read_len);
        switch (err) {
            case ESP_OK:
                ret = content;
            break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value %s is not initialized yet!", tag);
            break;
            default :
                ESP_LOGE(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
        }
        nvs_close(nvs_handle_);
    }

    return ret;
}

bool Nvs::openToRead() {
    return open(false);
}

bool Nvs::openToWrite() {
    return open(true);
}

bool Nvs::open(const bool write) {
    bool ret{true};

    const esp_err_t err{nvs_open(storage_name_, write ? NVS_READONLY :NVS_READWRITE, &nvs_handle_)};
    if (err != ESP_OK) {
        ESP_LOGE(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        ret = false;
    }

    return ret;
}