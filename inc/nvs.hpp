#pragma once

#include <optional>

#include <string>
#include <stdint.h>

#include <nvs.h>
#include <esp_system.h>
#include <nvs_flash.h>

using namespace std;

class Nvs {
    explicit Nvs(const char* const storage_name) : storage_name_{storage_name} {

    }

    public:
        std::optional<std::string> read(const char* const tag);
        std::optional<int64_t> read(const char* const tag);
        
        bool write(const char* const storage, const char* const tag, const int64_t val);
        bool write(const char* const storage, const char* const tag, const char* val);

        bool erase(const char* const storage, const char* const tag);
    private:
        bool openToRead();
        bool openToWrite();
        bool open(const bool write);
    private:
        static constexpr const char* const TAG{"Nvs"};
        const char* const storage_name_{""};
        nvs_handle nvs_handle_{};
};