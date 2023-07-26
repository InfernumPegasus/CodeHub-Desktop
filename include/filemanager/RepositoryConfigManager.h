#ifndef CODEHUB_REPOSITORYCONFIGMANAGER_H
#define CODEHUB_REPOSITORYCONFIGMANAGER_H

#include <filesystem>
#include <fstream>
#include "json/Json.h"

#include "IFileManager.h"
#include "utils/Types.h"

namespace fs = std::filesystem;

class RepositoryConfigManager : public IFileManager {
 public:
  RepositoryConfigManager(fs::path configFile, std::string* repositoryNameRef,
                          fs::path* repositoryFolderRef,
                          types::FileHashMap* fileHashMapRef,
                          std::string* currentBranchRef);

 public:
  bool Create() override;

  bool Read() override;

  void Update();

 private:
  const fs::path configFile_;

  std::string& repositoryNameRef_;
  fs::path& repositoryFolderRef_;
  types::FileHashMap& fileHashMapRef_;

  types::Branch& currentBranchRef_;
};

#endif  // CODEHUB_REPOSITORYCONFIGMANAGER_H
