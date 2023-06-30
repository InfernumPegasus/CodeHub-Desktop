#include "filemanager/RepositoryConfigManager.h"

#include <utility>

#include "config/ConfigFiles.h"
#include "serializer/JsonSerializer.h"

RepositoryConfigManager::RepositoryConfigManager(fs::path configFile,
                                                 std::string* repositoryNameRef,
                                                 fs::path* repositoryFolderRef,
                                                 FileHashMap* fileHashMapRef)
    : configFile_(std::move(configFile)),
      repositoryNameRef_(*repositoryNameRef),
      repositoryFolderRef_(*repositoryFolderRef),
      fileHashMapRef_(*fileHashMapRef) {}

bool RepositoryConfigManager::Create() {
  std::string configDirectory = repositoryFolderRef_ / CONFIG_DIRECTORY;
  std::ofstream file;
  if ((std::filesystem::exists(configDirectory) &&
       !std::filesystem::exists(configFile_)) ||
      std::filesystem::create_directory(configDirectory)) {
  }
  file.open(configFile_);
  return file.is_open();
}

bool RepositoryConfigManager::Read() {
  if (!std::filesystem::exists(configFile_)) {
    return false;
  }

  if (std::filesystem::is_empty(configFile_)) {
    return true;
  }

  std::ifstream ifs(configFile_);
  if (!ifs) return false;

  nlohmann::json j = nlohmann::json::parse(ifs);
  if (j.empty()) return false;

  const auto res = JsonSerializer::RepositoryFromConfigJson(j);
  repositoryNameRef_ = res->Name();
  repositoryFolderRef_ = res->Folder();
  fileHashMapRef_ = res->Map();

  return true;
}

void RepositoryConfigManager::Update() {
  std::ofstream ofs(configFile_);
  if (!ofs && !Create()) {
    return;
  }

  const auto repoJson =
      JsonSerializer::RepositoryToConfigJson(
          repositoryNameRef_, repositoryFolderRef_.c_str(), fileHashMapRef_)
          .dump(2);
  ofs << repoJson;
}
