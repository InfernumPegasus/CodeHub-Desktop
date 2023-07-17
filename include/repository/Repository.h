#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "commit/Commit.h"
#include "config/Types.h"
#include "filemanager/CommitsManager.h"
#include "filemanager/FilesManager.h"
#include "filemanager/IgnoreFileManager.h"
#include "filemanager/RepositoryConfigManager.h"

class Repository {
 public:
  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             std::string branch);

  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             types::FileHashMap files, std::string branch);

  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             const types::Commits& commits);

  ~Repository();

 public:
  types::FileHashMap ChangedFiles() const;

  types::FileHashMap RemovedFiles() const;

  types::FileHashMap CreatedFiles() const;

 private:
  types::FilesSet FilterCollectedFiles(const types::FileHashMap& collectedFiles);

 public:
  void DoCommit(const std::string& message);

  void SaveCommitFiles(const Commit& commit);

  static void RestoreCommitFiles(size_t checksum);

 public:
  void InitConfigManager();

  void InitFilesManager();

  void InitCommitsManager();

  void InitManagers();

 public:
  void ChangeBranch(std::string branch);

 public:
  [[nodiscard]] const std::string& Name() const;

  [[nodiscard]] const fs::path& Folder() const;

  [[nodiscard]] const types::Commits& Commits() const;

  [[nodiscard]] const std::string& CurrentBranch() const;

 private:
  std::string repositoryName_;
  fs::path repositoryFolder_;

  types::Branch currentBranch_;

  std::unique_ptr<RepositoryConfigManager> configManager_;
  std::unique_ptr<CommitsManager> commitsManager_;
  types::Commits commits_;

  std::unique_ptr<FilesManager> filesManager_;
  types::FileHashMap trackedFiles_;

  types::PathSet ignoredFiles_;
};

#endif  // CODEHUB_REPOSITORY_H
