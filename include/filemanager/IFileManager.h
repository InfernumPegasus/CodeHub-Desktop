#ifndef CODEHUB_IFILEMANAGER_H
#define CODEHUB_IFILEMANAGER_H

#include <pwd.h>
#include <unistd.h>

#include <filesystem>
#include <string>

#include "log/Logger.h"

class IFileManager {
 public:
  static std::filesystem::path GetHomeDirectory() {
    const auto dir = std::getenv("HOME");
    return dir == nullptr ? dir : getpwuid(getuid())->pw_dir;
  }

  virtual bool Create() = 0;

  virtual bool Read() = 0;

  virtual void Init() {
    if (!Read()) {
      logging::Log(LOG_WARNING, "IFileManager::Read() ended with error (first read)");
      if (!Create()) {
        throw std::runtime_error("IFileManager::Create() ended with error");
      }
    }
    if (!Read()) {
      throw std::runtime_error("IFileManager::Read() ended with error (final read)");
    }
  }
};

#endif  // CODEHUB_IFILEMANAGER_H
