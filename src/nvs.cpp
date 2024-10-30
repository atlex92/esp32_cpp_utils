#include "nvs.hpp"
#include <sys/unistd.h>
#include <string>
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"

std::optional<std::string> Nvs::readString(const char* const tag) const {

    std::optional<std::string> ret{std::nullopt};

    if (openToRead()) {
        std::string value{};
        esp_err_t err{doReadStr(tag, value)};
        if(ESP_OK == err) {
            ret = value;
        } else {
            handleError(tag, err);
        }
        close();
    }

    return ret;
}

std::optional<int8_t> Nvs::readI8(const char* const tag) const {
    return nvsRead<int8_t, &nvs_get_i8>(tag);
}

std::optional<int16_t> Nvs::readI16(const char* const tag) const {
    return nvsRead<int16_t, &nvs_get_i16>(tag);
}

std::optional<int32_t> Nvs::readI32(const char* const tag) const {
    return nvsRead<int32_t, &nvs_get_i32>(tag);
}

std::optional<int64_t> Nvs::readI64(const char* const tag) const {
    return nvsRead<int64_t, &nvs_get_i64>(tag);
}

std::optional<uint8_t> Nvs::readU8(const char* const tag) const {
    return nvsRead<uint8_t, &nvs_get_u8>(tag);
}

std::optional<uint16_t> Nvs::readU16(const char* const tag) const {
    return nvsRead<uint16_t, &nvs_get_u16>(tag);
}

std::optional<uint32_t> Nvs::readU32(const char* const tag) const {
    return nvsRead<uint32_t, &nvs_get_u32>(tag);
}

std::optional<uint64_t> Nvs::readU64(const char* const tag) const {
    return nvsRead<uint64_t, &nvs_get_u64>(tag);
}

bool Nvs::write(const char* const tag, const int8_t val) {
    return nvsWrite<int8_t, &nvs_set_i8>(tag, val);
}

bool Nvs::write(const char* const tag, const int16_t val) {
    return nvsWrite<int16_t, &nvs_set_i16>(tag, val);
}

bool Nvs::write(const char* const tag, const int32_t val) {
    return nvsWrite<int32_t, &nvs_set_i32>(tag, val);
}

bool Nvs::write(const char* const tag, const int64_t val) {
    return nvsWrite<int64_t, &nvs_set_i64>(tag, val);
}

bool Nvs::write(const char* const tag, const uint8_t val) {
    return nvsWrite<uint8_t, &nvs_set_u8>(tag, val);
}

bool Nvs::write(const char* const tag, const uint16_t val) {
    return nvsWrite<uint16_t, &nvs_set_u16>(tag, val);
}

bool Nvs::write(const char* const tag, const uint32_t val) {
    return nvsWrite<uint32_t, &nvs_set_u32>(tag, val);
}

bool Nvs::write(const char* const tag, const uint64_t val) {
    return nvsWrite<uint64_t, &nvs_set_u64>(tag, val);
}

bool Nvs::write(const char* const tag, const char* val) {
    return nvsWrite<const char*, &nvs_set_str>(tag, val);
}

bool Nvs::erase(const char* const tag) {

    bool ret{};

    if (openToWrite()) {
        const esp_err_t err{nvs_erase_key(nvs_handle_, tag)};
        if (ESP_OK != err ){
            ESP_LOGE(TAG, "failed to erase  %s value", tag);
            handleError(tag, err);
        } else if (commit()){
            ret = true;
        }
        close();
    }
    return ret;
}

bool Nvs::openToRead() const {
    return open(false);
}

bool Nvs::openToWrite() const {
    return open(true);
}

bool Nvs::open(const bool write) const {
    bool ret{true};

    const esp_err_t err{nvs_open(storage_name_, write ? NVS_READWRITE: NVS_READONLY, &nvs_handle_)};
    if (err != ESP_OK) {
        ESP_LOGE(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        ret = false;
    }

    return ret;
}

void Nvs::close() const {
    nvs_close(nvs_handle_);
}

void Nvs::handleError(const char* const tag, const esp_err_t err) const {
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

esp_err_t Nvs::doReadStr(const char* const tag, std::string& str) const {
    size_t read_len{};
    esp_err_t err{nvs_get_str(nvs_handle_, tag, nullptr, &read_len)};
    str = std::string{"", read_len};
    err = nvs_get_str(nvs_handle_, tag, str.data(), &read_len);
    return err;
}

bool Nvs::commit() {
    bool ret{true};
    if (ESP_OK != nvs_commit(nvs_handle_)) {
        ESP_LOGE(TAG, "failed to commit");
        ret = false;
    }
    return ret;
}