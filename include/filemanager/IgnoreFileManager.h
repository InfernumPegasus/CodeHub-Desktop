#ifndef CODEHUB_IGNOREFILEMANAGER_H
#define CODEHUB_IGNOREFILEMANAGER_H

#include <string>
#include <unordered_set>
#include <filesystem>

#include "IFileManager.h"

namespace fs = std::filesystem;

class IgnoreFileManager : public IFileManager {
 public:
  IgnoreFileManager(std::string_view repositoryFolder, std::string ignoreFile,
                    std::unordered_set<std::string>* ignoredFiles);

 public:
  bool Create() override;

  bool Read() override;

 public:
  static bool ShouldBeIgnored(std::string_view filename);

 private:
  const std::string_view repositoryFolder_;
  const std::string ignoreFile_;

  std::unordered_set<std::string>& ignoredFilesRef_;
};

#endif  // CODEHUB_IGNOREFILEMANAGER_H
