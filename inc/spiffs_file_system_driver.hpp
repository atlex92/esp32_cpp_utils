#pragma once

#include "Arduino.h"
#include "i_file_system_driver.hpp"

class SPIFFSFileSystemDriver : public AbstractFileSystemDriver {
public:
    SPIFFSFileSystemDriver();
    ~SPIFFSFileSystemDriver();
    std::vector<std::string> filesList(const std::string& path) const override;
    bool deleteFile(const std::string& full_file_path) const override;
    uint32_t countFiles (const std::string& path) const override;
    bool writeContentToFile (const std::string& content, const std::string& file_name) const override;
    bool appendContentToFile (const std::string& content, const std::string& file_name) const override;
    bool readEntireFileToString (const std::string& file_name, std::string& output) const override;
    bool doesFileExist(const std::string& file_name) const override;
    float usagePercent() const override;
    void initialize() override;
    bool format() const override;
private:
    bool printContentToStream(const std::string& content, Stream& stream) const;
    bool doWriteContentToFile(const std::string& content, const std::string& file_name, const bool append = false) const;
};
#endif