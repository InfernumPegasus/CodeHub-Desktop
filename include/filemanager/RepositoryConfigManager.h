#ifndef CODEHUB_REPOSITORYCONFIGMANAGER_H
#define CODEHUB_REPOSITORYCONFIGMANAGER_H

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "IFileManager.h"

namespace fs = std::filesystem;

class RepositoryConfigManager : public IFileManager {
 public:
  using FileHashMap = std::unordered_map<fs::path, size_t>;

 public:
  RepositoryConfigManager(fs::path configFile, std::string* repositoryNameRef,
                          fs::path* repositoryFolderRef, FileHashMap* fileHashMapRef);

 public:
  bool Create() override;

  bool Read() override;

  void Update();

 private:
  const fs::path configFile_;

  std::string& repositoryNameRef_;
  fs::path& repositoryFolderRef_;
  FileHashMap& fileHashMapRef_;
};

#endif  // CODEHUB_REPOSITORYCONFIGMANAGER_H
