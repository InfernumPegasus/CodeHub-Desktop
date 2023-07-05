#ifndef CODEHUB_FILESMANAGER_H
#define CODEHUB_FILESMANAGER_H

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include "IgnoreFileManager.h"

namespace fs = std::filesystem;

using FileHashMap = std::unordered_map<fs::path, size_t>;

class FilesManager {
 public:
  FilesManager(const fs::path& folder, FileHashMap* fileHashMap,
               const fs::path& ignoreFile, std::unordered_set<fs::path>* ignoredFiles);

 public:
  [[nodiscard]] FileHashMap ChangedFiles() const;

  [[nodiscard]] FileHashMap RemovedFiles() const;

  [[nodiscard]] FileHashMap CreatedFiles() const;

  [[nodiscard]] FileHashMap CollectFiles() const;

 private:
  const fs::path folder_;
  IgnoreFileManager ignoreFileManager_;
  FileHashMap& fileHashMapRef_;
};

#endif  // CODEHUB_FILESMANAGER_H
