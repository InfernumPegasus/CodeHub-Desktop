#ifndef CODEHUB_REPOSITORIESMANAGER_H
#define CODEHUB_REPOSITORIESMANAGER_H

#include <string>
#include <unordered_map>

#include "utils/Types.h"
#include "repository/Repository.h"

class RepositoriesManager : public IFileManager {
 public:
  explicit RepositoriesManager(types::NameFolderMap* nameAndFolderMap_);

 public:
  bool Create() override;

  bool Read() override;

  void Update();

 public:
  void DeleteIncorrectRepositories() const;

 private:
  const fs::path appConfigDirectory_;
  const fs::path repositoriesFile_;

  types::NameFolderMap& nameAndFolderMap_;
};

#endif  // CODEHUB_REPOSITORIESMANAGER_H
