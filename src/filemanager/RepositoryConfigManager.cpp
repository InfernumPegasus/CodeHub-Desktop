#include "filemanager/RepositoryConfigManager.h"

#include <iostream>
#include <utility>

#include "serializer/JsonSerializer.h"
#include "utils/ConfigFiles.h"

RepositoryConfigManager::RepositoryConfigManager(fs::path configFile,
                                                 std::string* repositoryNameRef,
                                                 fs::path* repositoryFolderRef,
                                                 types::FileHashMap* fileHashMapRef,
                                                 types::Branch* currentBranchRef)
    : configFile_(std::move(configFile)),
      repositoryNameRef_(*repositoryNameRef),
      repositoryFolderRef_(*repositoryFolderRef),
      fileHashMapRef_(*fileHashMapRef),
      currentBranchRef_(*currentBranchRef) {}

bool RepositoryConfigManager::Create() {
  const fs::path configDirectory = repositoryFolderRef_ / CONFIG_DIRECTORY;
  std::ofstream file;
  if ((fs::exists(configDirectory) && !fs::exists(configFile_)) ||
      fs::create_directory(configDirectory)) {
  }
  file.open(configFile_);
  return file.is_open();
}

bool RepositoryConfigManager::Read() {
  if (!fs::exists(configFile_)) {
    return false;
  }

  if (fs::is_empty(configFile_)) {
    return true;
  }

  std::ifstream ifs(configFile_);
  if (!ifs) {
    return false;
  }

  const nlohmann::json j = nlohmann::json::parse(ifs);
  if (j.empty()) {
    return false;
  }

  // TODO use JsonSerializer
  repositoryNameRef_ = j["repo_name"];
  repositoryFolderRef_ = j["repo_folder"].get<fs::path>();
  fileHashMapRef_ = j["map"];
  currentBranchRef_ = j["current_branch"];

  return true;
}

void RepositoryConfigManager::Update() {
  std::ofstream ofs(configFile_);
  if (!ofs && !Create()) {
    return;
  }

  const auto repoJson = JsonSerializer::RepositoryToConfigJson(
                            repositoryNameRef_, repositoryFolderRef_.c_str(),
                            fileHashMapRef_, currentBranchRef_)
                            .dump(2);
  ofs << repoJson;
}
