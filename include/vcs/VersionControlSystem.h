#ifndef CODEHUB_VERSIONCONTROLSYSTEM_H
#define CODEHUB_VERSIONCONTROLSYSTEM_H

#include <optional>

#include "filemanager/RepositoriesManager.h"
#include "filemanager/UserFileManager.h"
#include "repository/Repository.h"

using NameFolderMap = std::unordered_map<std::string, std::string>;

class VersionControlSystem {
 public:
  VersionControlSystem();

  ~VersionControlSystem();

  void Init();

 private:
  bool IsUniqueRepositoryData(const std::string& name, const fs::path& folder) const;

  void CheckRepositoriesExist() const;

 public:
  bool ExistsByName(std::string_view repositoryName) const;

  bool ExistsByFolder(const fs::path& repositoryFolder) const;

 public:
  void CreateRepository(std::string repositoryName);

 public:
  void CheckStatus() const;

  void DoCommit(std::string_view message);

  void Push();

  void ShowRepositories() const;

  void CommitsLog() const;

  void ShowFileDifference(std::string_view filename);

 public:
  void RestoreFiles(size_t checksum);

 public:
  void Clone(std::string_view repositoryName);

  void Pull();

 private:
  NameFolderMap nameFolderMap_;

  RepositoriesManager repositoriesManager_;

  UserFileManager userManager_;
};

#endif  // CODEHUB_VERSIONCONTROLSYSTEM_H
