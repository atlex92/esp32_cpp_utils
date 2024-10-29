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

    std::optional<std::string> readString(const char* const tag);
    std::optional<int64_t> readI64(const char* const tag);
    
    bool write(const char* const tag, const int64_t val);
    bool write(const char* const tag, const char* val);

    bool erase(const char* const tag);
private:
    bool open(const bool write);
    bool openToRead();
    bool openToWrite();
    void close();
    bool commit();
    void handleError(const char* const tag, const esp_err_t err);
    esp_err_t nvs_read(const char* const tag, std::string& str);
    esp_err_t nvs_read(const char* const tag, int64_t& val);
private:
    static constexpr const char* const TAG{"Nvs"};
    const char* const storage_name_{""};
    nvs_handle_t nvs_handle_{};
};