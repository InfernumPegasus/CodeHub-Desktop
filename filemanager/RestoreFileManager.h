#ifndef CODEHUB_RESTOREFILEMANAGER_H
#define CODEHUB_RESTOREFILEMANAGER_H

#include "../commit/Commit.h"
#include <filesystem>

namespace fs = std::filesystem;

class RestoreFileManager {
public:
    static bool CreateRecoveryFolder(const fs::path &folder);

    static void CopyFiles(const std::unordered_set<File> &files,
                          const fs::path &from,
                          const fs::path &to);

    static void CopyRecursive(const fs::path &from,
                              const fs::path &to);
};


#endif //CODEHUB_RESTOREFILEMANAGER_H
