#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "commit/Commit.h"
#include "config/RepositoryConfig.h"
#include "filemanager/BranchesManager.h"
#include "filemanager/CommitsManager.h"
#include "filemanager/FilesManager.h"
#include "filemanager/IgnoreFileManager.h"
#include "filemanager/RepositoryConfigManager.h"
#include "utils/Types.h"

class Repository {
 public:
  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             const std::string& branch);

  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             types::FileHashMap files, const std::string& branch);

  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             types::Commits commits);

  ~Repository();

 public:
  types::FileHashMap ChangedFiles() const;

  types::FileHashMap RemovedFiles() const;

  types::FileHashMap CreatedFiles() const;

 private:
  types::FilesSet FilterCollectedFiles(const types::FileHashMap& collectedFiles);

 public:
  void DoCommit(const std::string& message);

 public:
  void InitConfigManager();

  void InitFilesManager();

  void InitCommitsManager();

  void InitBranchesManager();

  void InitManagers();

 public:
  void ChangeBranch(std::string branch);

 public:
  [[nodiscard]] const std::string& Name() const;

  [[nodiscard]] const fs::path& Folder() const;

  [[nodiscard]] const types::Commits& Commits() const;

  [[nodiscard]] const std::string& CurrentBranch() const;

  [[nodiscard]] const types::Branches& Branches() const;

 private:
  RepositoryConfig config_;

  std::unique_ptr<RepositoryConfigManager> configManager_;
  std::unique_ptr<CommitsManager> commitsManager_;
  types::Commits commits_;

  std::unique_ptr<FilesManager> filesManager_;
  types::FileHashMap trackedFiles_;

  types::PathSet ignoredFiles_;

  std::unique_ptr<BranchesManager> branchesManager_;
};

#endif  // CODEHUB_REPOSITORY_H
