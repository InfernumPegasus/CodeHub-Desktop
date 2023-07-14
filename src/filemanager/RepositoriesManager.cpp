#include "filemanager/RepositoriesManager.h"

#include <filesystem>
#include <iostream>

#include "config/ConfigFiles.h"
#include "serializer/JsonSerializer.h"

RepositoriesManager::RepositoriesManager(types::NameFolderMap* nameAndFolderMap_)
    : appConfigDirectory_(GetHomeDirectory()),
      repositoriesFile_(appConfigDirectory_ / VCS_REPOSITORIES_FILE),
      nameAndFolderMap_(*nameAndFolderMap_) {}

bool RepositoriesManager::Create() {
  std::ofstream file;
  if ((std::filesystem::exists(appConfigDirectory_) &&
       !std::filesystem::exists(repositoriesFile_)) ||
      std::filesystem::create_directory(appConfigDirectory_)) {
  }
  file.open(repositoriesFile_);
  return file.is_open();
}

bool RepositoriesManager::Read() {
  if (!std::filesystem::exists(repositoriesFile_)) {
    return false;
  }

  if (std::filesystem::is_empty(repositoriesFile_)) {
    return true;
  }

  std::ifstream ifs(repositoriesFile_);
  if (!ifs) return false;

  const nlohmann::json j = nlohmann::json::parse(ifs);
  nameAndFolderMap_ = j["map"];  // JsonSerializer::NameFolderFromJson(j);
  std::erase_if(nameAndFolderMap_, [](const auto& kv) {
    return !std::filesystem::exists(kv.second / CONFIG_DIRECTORY);
  });

  return true;
}

void RepositoriesManager::Update() {
  std::ofstream ofs(repositoriesFile_);
  if (!ofs && !Create()) {
    std::cout << "Cannot create global config folder!\n";
    return;
  }

  nlohmann::json json;
  json["map"] = nameAndFolderMap_;
  ofs << json.dump(2);
}
