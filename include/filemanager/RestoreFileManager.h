#ifndef CODEHUB_RESTOREFILEMANAGER_H
#define CODEHUB_RESTOREFILEMANAGER_H

#include <filesystem>

#include "commit/Commit.h"

namespace fs = std::filesystem;

class RestoreFileManager {
 public:
  static void CopyFiles(const types::FilesSet& files, const fs::path& from,
                        const fs::path& to);

  static void CopyFolderRecursive(const fs::path& from, const fs::path& to);
};

#endif  // CODEHUB_RESTOREFILEMANAGER_H
