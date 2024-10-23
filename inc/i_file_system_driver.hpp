#pragma once

/***
 * @file i_file_system_driver.hpp
 * @brief provides interface for file system driver classes
 */

#include <string>
#include <vector>

/**
 * @class IFileSystemDriver
 * Interface class for file system drivers
 */
class IFileSystemDriver {
public:
  /**
   * Virtual destructor
   */
  virtual ~IFileSystemDriver() = default;

  /**
   * Perform write operation into a file
   * @param content data to write
   * @param file_name file to write into (full file path)
   * @return true if successful, otherwise false
   */
  virtual bool writeContentToFile(const std::string& content, const std::string& file_name) const = 0;

  /**
   * Get free space on file system
   * @return percent of free space
   */
  virtual float usagePercent() const = 0;
  virtual bool format() const = 0;
  virtual bool deleteFile(const std::string& file_path) const = 0;
  virtual std::vector<std::string> filesList(const std::string& path) const = 0;
  virtual uint32_t countFiles(const std::string& path) const = 0;
  virtual bool readEntireFileToString(const std::string& file_name, std::string& output) const = 0;
  virtual bool appendContentToFile(const std::string& content, const std::string& file_name) const = 0;
  virtual bool doesFileExist(const std::string& file_name) const = 0;
};