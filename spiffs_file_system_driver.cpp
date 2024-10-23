// #include <SPIFFS.h>
// #include <Stream.h>
// #include <md5.hpp>
// #include "spiffs_file_system_driver.hpp"
// #include "file_path_utils.hpp"
// #include "esp_spiffs.h"

// static const char* const TAG {"SPIFFS_DRIVER"};

// static const uint32_t kMaxFileNameLength {31};

// SPIFFSFileSystemDriver::SPIFFSFileSystemDriver() {
// }

// SPIFFSFileSystemDriver::~SPIFFSFileSystemDriver() {

// }

// bool SPIFFSFileSystemDriver::initializeImpl() {
//     bool ret{true};
//     if (false == SPIFFS.begin()) {
//         ESP_LOGE(TAG, "Failed to begin SPIFFS, formatting now...");
//         SPIFFS.format();
//         ret = SPIFFS.begin();
//     }
//     return ret;
// }

// bool SPIFFSFileSystemDriver::writeContentToFile (const std::string& content, const std::string& filename) const {
//     if (filename.length() > kMaxFileNameLength) {
//         ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
//         return false;
//     }
//     return writeContentToFileImpl(content, filename, false);
// }

// bool SPIFFSFileSystemDriver::appendContentToFile (const string& content, const string& filename) const {
//     if (filename.length() > kMaxFileNameLength) {
//         ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
//         return false;
//     }
//     return writeContentToFileImpl(content, filename, true);
// }

// uint32_t SPIFFSFileSystemDriver::countFiles (const string& path) const {
//     uint32_t ret {};
//     File root{SPIFFS.open(path.c_str())};

//     if (!root) {
//         ESP_LOGE(TAG, "Failed to open path");
//         root.close();
//     } else {
//         File file{root.openNextFile()};
//         while (file) {
//             ++ret;
//             file.close();
//             file = root.openNextFile();
//         }
//         root.close();
//         file.close();
//     }

//     return ret;
// }

// vector<std::string> SPIFFSFileSystemDriver::filesList(const std::string& path) const {

//     std::vector<std::string> ret;

//     File root{SPIFFS.open(path.c_str())};
//     if (!root) {
//         ESP_LOGE(TAG, "Failed to open path: %s", path.c_str());
//         root.close();
//     } else {
//         File file{root.openNextFile()};
//         while (file) {
//             ret.push_back(file.name());
//             file.close();
//             file = root.openNextFile();
//         }
//         root.close();
//         file.close();
//     }

//     return ret;
// }

// bool SPIFFSFileSystemDriver::readEntireFileToString (const std::string& filename, std::string& output) const {

//     if (filename.length() > kMaxFileNameLength) {
//         ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
//         return false;
//     }

//     if (!doesFileExist(filename)) {
//         return false;
//     }

//     File file{SPIFFS.open(filename.c_str(), FILE_READ)};

//     if (!file) {
//         return false;
//     }

//     const size_t fileSize{file.size()};
//     vector<char> tempBuffer{};
//     tempBuffer.resize(fileSize, '\0');

//     const size_t bytesRead = file.readBytes(const_cast<char*>(tempBuffer.data()), fileSize);
//     file.close();

//     if (fileSize == bytesRead) {
//         output = std::string(tempBuffer.begin(), tempBuffer.end());
//         return true;
//     } else {
//         return false;
//     }
// }

// bool SPIFFSFileSystemDriver::deleteFile(const std::string& filename) const {

//     if (filename.length() > kMaxFileNameLength) {
//         ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
//         return false;
//     }

//     if (false == doesFileExist(filename)) {
//         return true;
//     }

//     if (true == SPIFFS.remove(filename.c_str())) {
//         return true;
//     } else {
//         return false;
//     }
// }

// bool SPIFFSFileSystemDriver::doesFileExist(const std::string& filename) const {

//     if (filename.length() > kMaxFileNameLength) {
//         ESP_LOGE(TAG, "max filename length exceeded: %s!", filename.c_str());
//         return false;
//     }
//     return SPIFFS.exists(filename.c_str());
// }

// bool SPIFFSFileSystemDriver::writeContentToFileImpl(const std::string& content, const std::string& filename, const
// bool append) const {

//     bool ret{false};
//     const char* const mode_key{append? FILE_APPEND : FILE_WRITE};
//     File file{SPIFFS.open(filename.c_str(), mode_key)};

//     if (file){
//         ret = printContentToStream(content, file);
//     }
//     else {
//         ESP_LOGE(TAG, "failed to open file: %s", filename.c_str());
//     }
//     file.close();
//     return ret;
// }

// bool SPIFFSFileSystemDriver::printContentToStream(const std::string& content, Stream& stream) const {
//     const size_t printedBytes{stream.write(content.data(), content.length())};
//     if(printedBytes != content.length()) {
//         ESP_LOGE(TAG, "error while printing content to Stream!");
//         return false;
//     }
//     return true;
// }

// bool SPIFFSFileSystemDriver::format() const {
//     const bool ret{SPIFFS.format()};
//     if(!ret) {
//         ESP_LOGE(TAG, "failed to format file system");
//     }
//     return ret;
// }

// float SPIFFSFileSystemDriver::usagePercent() const {
//     const size_t totalBytes {SPIFFS.totalBytes()};
//     const size_t usedBytes {SPIFFS.usedBytes()};
//     const float usagePercent {100.0F * static_cast<float>(usedBytes) / totalBytes};
//     return usagePercent;
// }
