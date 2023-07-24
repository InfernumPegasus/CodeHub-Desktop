#ifndef CODEHUB_BRANCHESMANAGER_H
#define CODEHUB_BRANCHESMANAGER_H

#include <filesystem>

#include "utils/Types.h"
#include "filemanager/IFileManager.h"

namespace fs = std::filesystem;

class BranchesManager : public IFileManager {
 public:
  BranchesManager(std::string* repositoryNameRef, types::Branches* branchesRef);

 public:
  bool Create() override;

  bool Read() override;

  void Update() const;

 private:
  std::string& repositoryNameRef_;
  types::Branches& branchesRef_;

  const fs::path branchesFile_;
};

#endif  // CODEHUB_BRANCHESMANAGER_H
