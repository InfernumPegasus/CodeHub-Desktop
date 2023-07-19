#include "filemanager/RepositoriesManager.h"

#include <filesystem>
#include <iostream>

#include "config/ConfigFiles.h"
#include "log/Logger.h"

RepositoriesManager::RepositoriesManager(types::NameFolderMap* nameAndFolderMap_)
    : appConfigDirectory_(GetHomeDirectory() / VCS_CONFIG_FOLDER),
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

void RepositoriesManager::DeleteIncorrectRepositories() const {
  const auto removeFunc = [this](const fs::path& folder) {
    if (fs::is_directory(folder)) {
      const auto res = fs::remove_all(folder);
      logging::Log(LOG_WARNING,
                   fmt::format("Folder '{}' with {} files deleted from '{}'",
                               folder.c_str(), res, appConfigDirectory_.c_str()));
    }
  };
  if (!nameAndFolderMap_.empty()) {
    for (const auto& [name, folder] : nameAndFolderMap_) {
      const auto dir = appConfigDirectory_ / name;
      if (!fs::exists(dir)) {
        removeFunc(appConfigDirectory_ / name);
      }
    }
  } else {
    for (const auto& folder : fs::directory_iterator(appConfigDirectory_)) {
      removeFunc(folder);
    }
  }
}
