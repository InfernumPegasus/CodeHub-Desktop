#ifndef CODEHUB_VERSIONCONTROLSYSTEM_H
#define CODEHUB_VERSIONCONTROLSYSTEM_H

#include "filemanager/UserFileManager.h"
#include "repository/Repository.h"
#include "utils/Types.h"

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

  void ShowFileDifference(const fs::path& filename);

 public:
  void RestoreFiles(size_t checksum);

 public:
  void CreateBranch(std::string name);

  void ShowBranches() const;

 private:
  std::string GetRepositoryNameByFolder(const fs::path& folder) const;

  static void CreateRepositoryConfigs(const std::string& repositoryName);

 private:
  types::NameFolderMap nameFolderMap_;

  std::unique_ptr<UserFileManager> userManager_;
};

#endif  // CODEHUB_VERSIONCONTROLSYSTEM_H
