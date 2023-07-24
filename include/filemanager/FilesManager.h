#ifndef CODEHUB_FILESMANAGER_H
#define CODEHUB_FILESMANAGER_H

#include <filesystem>
#include <memory>

#include "IgnoreFileManager.h"
#include "utils/Types.h"

namespace fs = std::filesystem;

class FilesManager {
 public:
  FilesManager(const fs::path& folder, types::FileHashMap* fileHashMap,
               const fs::path& ignoreFile, types::PathSet* ignoredFiles);

 public:
  void Init();

  [[nodiscard]] types::FileHashMap ChangedFiles() const;

  [[nodiscard]] types::FileHashMap RemovedFiles() const;

  [[nodiscard]] types::FileHashMap CreatedFiles() const;

  [[nodiscard]] types::FileHashMap CollectFiles() const;

 private:
  const fs::path folder_;
  std::unique_ptr<IgnoreFileManager> ignoreFileManager_;
  types::FileHashMap& fileHashMapRef_;
};

#endif  // CODEHUB_FILESMANAGER_H
