#include "nvs.hpp"
#include <sys/unistd.h>
#include <string>
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"

std::optional<std::string> Nvs::readString(const char* const tag) {

    std::optional<std::string> ret{std::nullopt};

    if (openToRead()) {
        std::string value{};
        esp_err_t err{nvs_read(tag, value)};
        if(ESP_OK == err) {
            ret = value;
        } else {
            handleError(tag, err);
        }
        nvs_close(nvs_handle_);
    }

    return ret;
}

std::optional<int64_t> Nvs::readI64(const char* const tag) {

    std::optional<int64_t> ret{std::nullopt};

    if (openToRead()) {
        int64_t value{};
        esp_err_t err{nvs_read(tag, value)};
        if(ESP_OK == err) {
            ret = value;
        } else {
            handleError(tag, err);
        }
        nvs_close(nvs_handle_);
    }

    return ret;
}

bool Nvs::write(const char* const tag, const int64_t val) {
    bool ret{};

    if (openToWrite()) {
        esp_err_t err = nvs_set_i64(nvs_handle_, tag, val);
        if (err != ESP_OK){
            ESP_LOGE(TAG, "failed to set int64_t value %s", tag);
        } else if (commit()){
            ret = true;
        }
        close();
    }

    return ret;
}

bool Nvs::write(const char* const tag, const char* val) {
    bool ret{};

    if (openToWrite()) {
        esp_err_t err = nvs_set_str(nvs_handle_, tag, val);
        if (err != ESP_OK){
            ESP_LOGE(TAG, "failed to set string value %s", tag);
        } else if (commit()){
            ret = true;
        }
        close();
    }

    return ret;
}

bool Nvs::erase(const char* const tag) {

    bool ret{};

    if (openToWrite()) {
        esp_err_t err{nvs_erase_key(nvs_handle_, tag)};
        if (ESP_OK != err ){
            ESP_LOGE(TAG, "failed to erase  %s value", tag);
        } else if (commit()){
            ret = true;
        }
        close();
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

    const esp_err_t err{nvs_open(storage_name_, write ? NVS_READWRITE: NVS_READONLY, &nvs_handle_)};
    if (err != ESP_OK) {
        ESP_LOGE(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        ret = false;
    }

    return ret;
}

void Nvs::close() {
    nvs_close(nvs_handle_);
}

void Nvs::handleError(const char* const tag, const esp_err_t err) {
    switch (err) {
        case ESP_OK:
        break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(TAG, "The value %s is not initialized yet!", tag);
        break;
        default :
            ESP_LOGE(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
    }
}

esp_err_t Nvs::nvs_read(const char* const tag, std::string& str) {
    size_t read_len{};
    esp_err_t err{nvs_get_str(nvs_handle_, tag, nullptr, &read_len)};
    str = std::string{"", read_len};
    err = nvs_get_str(nvs_handle_, tag, str.data(), &read_len);
    return err;
}

esp_err_t Nvs::nvs_read(const char* const tag, int64_t& val) {
    return nvs_get_i64(nvs_handle_, tag, &val);
}

bool Nvs::commit() {
    bool ret{true};
    if (ESP_OK != nvs_commit(nvs_handle_)) {
        ESP_LOGE(TAG, "failed to commit");
        ret = false;
    }
    return ret;
}