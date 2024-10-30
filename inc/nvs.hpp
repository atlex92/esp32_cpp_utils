#pragma once

#include <optional>
#include <string>
#include <stdint.h>

#include <nvs.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <esp_err.h>
class Nvs {
public:
    explicit Nvs(const char* const storage_name) : storage_name_{storage_name} {
    }

    std::optional<std::string> readString(const char* const tag) const;

    std::optional<int8_t> readI8(const char* const tag) const;
    std::optional<int16_t> readI16(const char* const tag) const;
    std::optional<int32_t> readI32(const char* const tag) const;
    std::optional<int64_t> readI64(const char* const tag) const;

    std::optional<uint8_t> readU8(const char* const tag) const;
    std::optional<uint16_t> readU16(const char* const tag) const;
    std::optional<uint32_t> readU32(const char* const tag) const;
    std::optional<uint64_t> readU64(const char* const tag) const;

    bool write(const char* const tag, const int8_t val);
    bool write(const char* const tag, const int16_t val);
    bool write(const char* const tag, const int32_t val);
    bool write(const char* const tag, const int64_t val);

    bool write(const char* const tag, const uint8_t val);
    bool write(const char* const tag, const uint16_t val);
    bool write(const char* const tag, const uint32_t val);
    bool write(const char* const tag, const uint64_t val);

    bool write(const char* const tag, const char* val);

    bool erase(const char* const tag);
private:
    bool open(const bool write) const;
    bool openToRead() const;
    bool openToWrite() const;
    void close() const;
    bool commit();
    void handleError(const char* const tag, const esp_err_t err) const;
    esp_err_t doReadStr(const char* const tag, std::string& str) const;

    template <typename T>
    using nvs_read_func_t = esp_err_t(*)(nvs_handle_t, const char*, T*);

    template <typename T>
    using nvs_write_func_t = esp_err_t(*)(nvs_handle_t, const char*, T);

    template <typename T, nvs_read_func_t<T> nvs_read_func>
    std::optional<T> nvsRead(const char* tag) const {
        std::optional<T> ret{std::nullopt};
        if (openToRead()) {
            T val{};
            const esp_err_t err{nvs_read_func(nvs_handle_, tag, &val)};
            if(ESP_OK == err) {
                ret = val;
            } else {
                handleError(tag, err);
            }
            close();
        }
        return ret;
    }

    template <typename T, nvs_write_func_t<T> nvs_write_func>
    bool nvsWrite(const char* tag, const T val) {
        bool ret{false};
        if (openToWrite()) {
            const esp_err_t err{nvs_write_func(nvs_handle_, tag, val)};
            if (err != ESP_OK){
                handleError(tag, err);
            } else if (commit()){
                ret = true;
            }
            close();
        }
        return ret;
    }
private:
    static constexpr const char* const TAG{"Nvs"};
    const char* const storage_name_{""};
    mutable nvs_handle_t nvs_handle_{};
};