#ifndef CODEHUB_VERSIONCONTROLSYSTEM_H
#define CODEHUB_VERSIONCONTROLSYSTEM_H

#include <optional>

#include "config/Types.h"
#include "filemanager/RepositoriesManager.h"
#include "filemanager/UserFileManager.h"
#include "repository/Repository.h"

class VersionControlSystem {
 public:
  VersionControlSystem();

  ~VersionControlSystem();

  void Init();

 private:
  void CheckRepositoriesExist() const;

 public:
  bool ExistsByName(std::string_view repositoryName) const;

  bool ExistsByFolder(const fs::path& repositoryFolder) const;

 public:
  void CreateRepository(std::string repositoryName);

 public:
  void CheckStatus() const;

  void DoCommit(const std::string& message);

  void Push();

  void ShowRepositories() const;

  void CommitsLog() const;

  void ShowFileDifference(std::string_view filename);

 public:
  void RestoreFiles(size_t checksum);

 private:
  types::NameFolderMap nameFolderMap_;

  RepositoriesManager repositoriesManager_;

  UserFileManager userManager_;
};

#endif  // CODEHUB_VERSIONCONTROLSYSTEM_H
