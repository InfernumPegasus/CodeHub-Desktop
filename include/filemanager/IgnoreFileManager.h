#ifndef CODEHUB_IGNOREFILEMANAGER_H
#define CODEHUB_IGNOREFILEMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_set>

#include "IFileManager.h"
#include "utils/Types.h"

namespace fs = std::filesystem;

// TODO add regexes support
class IgnoreFileManager : public IFileManager {
 public:
  IgnoreFileManager(fs::path repositoryFolder, fs::path ignoreFile,
                    types::PathSet* ignoredFilesRef);

 public:
  bool Create() override;

  bool Read() override;

 public:
  [[nodiscard]] bool ShouldBeIgnored(const std::string& filename) const;

 private:
  const fs::path repositoryFolder_;
  const fs::path ignoreFile_;

  types::PathSet& ignoredFilesRef_;
};

#endif  // CODEHUB_IGNOREFILEMANAGER_H
