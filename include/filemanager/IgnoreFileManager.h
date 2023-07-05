#ifndef CODEHUB_IGNOREFILEMANAGER_H
#define CODEHUB_IGNOREFILEMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_set>

#include "IFileManager.h"

namespace fs = std::filesystem;

class IgnoreFileManager : public IFileManager {
 public:
  IgnoreFileManager(std::string_view repositoryFolder, std::string ignoreFile,
                    std::unordered_set<fs::path>* ignoredFiles);

 public:
  bool Create() override;

  bool Read() override;

 public:
  [[nodiscard]] bool ShouldBeIgnored(std::string_view filename) const;

 private:
  const std::string_view repositoryFolder_;
  const std::string ignoreFile_;

  std::unordered_set<fs::path>& ignoredFilesRef_;
};

#endif  // CODEHUB_IGNOREFILEMANAGER_H
