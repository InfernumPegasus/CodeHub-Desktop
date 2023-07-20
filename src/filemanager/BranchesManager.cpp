#include "filemanager/BranchesManager.h"

#include "config/ConfigFiles.h"
#include "log/Logger.h"
#include "serializer/JsonSerializer.h"

BranchesManager::BranchesManager(std::string* repositoryNameRef,
                                 types::Branches* branchesRef)
    : repositoryNameRef_(*repositoryNameRef),
      branchesRef_(*branchesRef),
      branchesFile_(GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryNameRef_ /
                    BRANCHES_FILE) {}

bool BranchesManager::Create() {
  std::ofstream ofs(branchesFile_);
  const auto isOpen = ofs.is_open();
  if (!isOpen) {
    logging::Log(LOG_WARNING, fmt::format("Branches file '{}' cannot be opened",
                                          branchesFile_.c_str()));
    return isOpen;
  }
  ofs << JsonSerializer::BranchesToJson(branchesRef_);
  return isOpen;
}

bool BranchesManager::Read() {
  if (!fs::exists(branchesFile_) || fs::is_empty(branchesFile_)) {
    return false;
  }

  std::ifstream ifs(branchesFile_);
  if (!ifs) {
    return false;
  }

  nlohmann::json j = nlohmann::json::parse(ifs);
  branchesRef_ = JsonSerializer::BranchesFromJson(j);
  return ifs.is_open();
}

void BranchesManager::Update() const {
  std::ofstream ofs(branchesFile_);
  if (!ofs) {
    return;
  }

  const auto repoJson = JsonSerializer::BranchesToJson(branchesRef_).dump(2);
  ofs << repoJson;
}
