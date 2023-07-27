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

  fs::path FormBranchFolder() const;

  static fs::path FormRepositoryFolderPath(const std::string& repositoryName);

  nlohmann::json ToJson() const;

 public:
  std::string repositoryName_;
  fs::path repositoryFolder_;

  types::Branch currentBranch_;
  types::Branches branches_;
};

RepositoryConfig RepositoryConfigFromFile(const fs::path& configPath);

void CheckRepositoryConfig(const RepositoryConfig& config);

types::FileHashMap ReadTrackedFromFile(const fs::path& path);

types::Commits ReadCommitsFromFile(const fs::path& path);

#endif  // CODEHUB_REPOSITORYCONFIG_H
