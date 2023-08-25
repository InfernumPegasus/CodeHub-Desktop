#ifndef CODEHUB_REPOSITORYCONFIG_H
#define CODEHUB_REPOSITORYCONFIG_H

#include <filesystem>
#include <ostream>
#include <string>

#include "commit/Commit.h"
#include "filemanager/IFileManager.h"
#include "utils/Types.h"

struct RepositoryConfig {
  fs::path FormCommittedFilesSavePath(size_t commitChecksum) const;

  static fs::path FormRepositoryConfigFilePath(const std::string& repositoryName);

  nlohmann::json ToJson() const;

  void CheckBranchExist(const types::Branch& branch) const;

 public:
  std::string repositoryName_;
  fs::path repositoryFolder_;

  types::Branch currentBranch_;
  types::Branches branches_;
};

RepositoryConfig ReadRepositoryConfigFromFile(const fs::path& path);

void CheckRepositoryConfig(const RepositoryConfig& config);

types::FileHashMap ReadTrackedFromFile(const fs::path& path);

types::Commits ReadCommitsFromFile(const fs::path& path);

void CreateConfigFile(const fs::path& path, const std::string& defaultValue);

void CreateFolder(const fs::path& path);

#endif  // CODEHUB_REPOSITORYCONFIG_H
