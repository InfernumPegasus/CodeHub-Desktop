#include "RepositoriesManager.h"
#include "../serializer/JsonSerializer.h"
#include "../config/Configs.h"
#include <pwd.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>

RepositoriesManager::RepositoriesManager(
        RepositoriesManager::NameFolderMap *nameAndFolderMap_) :
        appConfigDirectory_(GetHomeDirectory()),
        repositoriesFile_(appConfigDirectory_ + "/" + VCS_REPOSITORIES_FILE),
        nameAndFolderMap_(*nameAndFolderMap_) {}


std::string RepositoriesManager::GetHomeDirectory() {
    auto dir = std::getenv("HOME");
    return dir == nullptr ? dir : getpwuid(getuid())->pw_dir;
}

bool RepositoriesManager::Create() {
    std::ofstream file;
    if ((std::filesystem::exists(appConfigDirectory_) &&
         !std::filesystem::exists(repositoriesFile_)) ||
        std::filesystem::create_directory(appConfigDirectory_)) {}
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

    nlohmann::json j = nlohmann::json::parse(ifs);
    nameAndFolderMap_ = JsonSerializer::NameFolderFromJson(j);
    std::erase_if(nameAndFolderMap_,
                  [](auto &kv) {
                      return !std::filesystem::exists(
                              kv.second + "/" + VCS_CONFIG_DIRECTORY
                      );
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
