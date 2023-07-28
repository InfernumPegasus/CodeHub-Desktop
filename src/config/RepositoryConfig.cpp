#include "config/RepositoryConfig.h"

#include <fmt/format.h>

#include <fstream>

#include "json/JsonSerializer.h"
#include "utils/ConfigFiles.h"

fs::path RepositoryConfig::FormCommittedFilesSavePath(size_t commitChecksum) const {
  return GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName_ / currentBranch_ /
         std::to_string(commitChecksum);
}

fs::path RepositoryConfig::FormBranchFolder() const {
  return GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName_ / currentBranch_;
}

nlohmann::json RepositoryConfig::ToJson() const {
  nlohmann::json j;
  j["repo_name"] = repositoryName_;
  j["repo_folder"] = repositoryFolder_;
  j["current_branch"] = currentBranch_;
  j["branches"] = branches_;
  return j;
}

fs::path RepositoryConfig::FormRepositoryFolderPath(const std::string& repositoryName) {
  return GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName / REPOSITORY_CONFIG_FILE;
}

RepositoryConfig ReadRepositoryConfigFromFile(const fs::path& path) {
  std::ifstream ifs(path);
  if (!ifs) {
    throw std::runtime_error(
        fmt::format("Repositories file '{}' cannot be opened", path.c_str()));
  }
  const auto json = nlohmann::json::parse(ifs);
  return {json["repo_name"], json["repo_folder"], json["current_branch"],
          json["branches"]};
}

void CheckRepositoryConfig(const RepositoryConfig& config) {
  if (config.repositoryName_.empty() || config.repositoryFolder_.empty()) {
    throw std::runtime_error("Repository name or folder cannot be empty");
  }

  if (config.branches_.empty()) {
    throw std::runtime_error("Branches cannot be empty");
  }

  if (config.currentBranch_.empty()) {
    throw std::runtime_error("No current branch selected");
  }
}

types::FileHashMap ReadTrackedFromFile(const fs::path& path) {
  std::ifstream ifs(path);
  if (!ifs) {
    throw std::runtime_error(
        fmt::format("Tracked file '{}' cannot be opened", path.c_str()));
  }
  const auto json = nlohmann::json::parse(ifs);
  return json["tracked_files"];
}

types::Commits ReadCommitsFromFile(const fs::path& path) {
  std::ifstream ifs(path);
  if (!ifs) {
    throw std::runtime_error(
        fmt::format("Commits file '{}' cannot be opened", path.c_str()));
  }
  const auto json = nlohmann::json::parse(ifs);
  return JsonSerializer::CommitsFromJson(json);
}

void CreateConfigFile(const fs::path& path, const std::string& defaultValue) {
  if (!fs::exists(path)) {
    std::ofstream file(path);
    if (!file) {
      throw std::runtime_error(fmt::format("Cannot create file '{}'", path.c_str()));
    }
    file << defaultValue;
  }
}

void CreateFolder(const fs::path& path) {
  if (!fs::exists(path) && !fs::create_directories(path)) {
    throw std::runtime_error(
        fmt::format("Cannot create repository folder in '{}'", path.c_str()));
  }
}
