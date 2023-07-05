#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include <list>
#include <unordered_map>
#include <unordered_set>

#include "commit/Commit.h"
#include "filemanager/CommitsManager.h"
#include "filemanager/FilesManager.h"
#include "filemanager/IgnoreFileManager.h"
#include "filemanager/RepositoryConfigManager.h"

class Repository {
 public:
  Repository(std::string repositoryName, const fs::path& repositoryFolder);

  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             FileHashMap files);

  Repository(std::string repositoryName, const fs::path& repositoryFolder,
             const std::list<Commit>& commits);

  ~Repository();

 public:
  FileHashMap ChangedFiles() const;

  FileHashMap RemovedFiles() const;

  FileHashMap CreatedFiles() const;

 private:
  std::unordered_set<File> FilterCollectedFiles(const FileHashMap& collectedFiles);

 public:
  void DoCommit(std::string_view message);

  static std::tuple<int, int, int> CountFilesStatuses(
      const std::unordered_set<File>& files);

  static void SaveCommitFiles(const Commit& commit);

  static void RestoreCommitFiles(size_t checksum);

 public:
  void InitConfigManager();

  //  void InitIgnoreManager();

  void InitCommitsManager();

  void InitManagers();

 public:
  [[nodiscard]] const std::string& Name() const;

  [[nodiscard]] const fs::path& Folder() const;

  [[nodiscard]] const std::list<Commit>& Commits() const;

  [[nodiscard]] const FileHashMap& Map() const;

 private:
  std::string repositoryName_;
  fs::path repositoryFolder_;

  RepositoryConfigManager configManager_;
  CommitsManager commitsManager_;
  FilesManager filesManager_;
  //  IgnoreFileManager ignoreFileManager_;

  std::list<Commit> commits_;
  std::unordered_set<fs::path> ignoredFiles_;

  FileHashMap fileHashMap_;
};

#endif  // CODEHUB_REPOSITORY_H
