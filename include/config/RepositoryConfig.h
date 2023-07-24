#ifndef CODEHUB_REPOSITORYCONFIG_H
#define CODEHUB_REPOSITORYCONFIG_H

#include <filesystem>
#include <string>

#include "filemanager/IFileManager.h"
#include "utils/Types.h"

struct RepositoryConfig {
  fs::path FormCommittedFilesSavePath(size_t commitChecksum) const;

  fs::path FormBranchFolder() const;

  std::string repositoryName_;
  fs::path repositoryFolder_;

  types::Branch currentBranch_;
  types::Branches branches_;
};

static RepositoryConfig ReadRepositoryConfig();

static void VerifyRepositoryConfig(const RepositoryConfig& config);

#endif  // CODEHUB_REPOSITORYCONFIG_H
