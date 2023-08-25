#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "commit/Commit.h"
#include "config/RepositoryConfig.h"
#include "filemanager/FilesManager.h"
#include "utils/Types.h"

inline constexpr auto DEFAULT_BRANCH_NAME = "master";

class Repository {
 public:
  explicit Repository(RepositoryConfig config);

  ~Repository();

 public:
  types::FileHashMap ChangedFiles() const;

  types::FileHashMap RemovedFiles() const;

  types::FileHashMap CreatedFiles() const;

 private:
  types::FilesSet FilterCollectedFiles(const types::FileHashMap& collectedFiles);

 public:
  void DoCommit(const std::string& message);

  void Init();

 public:
  void AddBranch(const types::Branch& branch);

  void ChangeBranch(types::Branch branch);

  void MergeBranch(const types::Branch& mergeWith);

 public:
  [[nodiscard]] const std::string& Name() const;

  [[nodiscard]] const types::Commits& Commits() const;

  [[nodiscard]] const types::Branch& CurrentBranch() const;

  [[nodiscard]] const types::Branches& Branches() const;

 private:
  RepositoryConfig config_;

  types::Commits commits_;

  std::unique_ptr<FilesManager> filesManager_;
  types::FileHashMap trackedFiles_;

  types::PathSet ignoredFiles_;
};

#endif  // CODEHUB_REPOSITORY_H
