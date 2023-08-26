#include "config/RepositoryConfig.h"

#include <fmt/format.h>

#include <fstream>

#include "json/JsonSerializer.h"
#include "utils/ConfigFiles.h"

fs::path RepositoryConfig::FormCommittedFilesSavePath(size_t commitChecksum) const {
  return GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName_ / currentBranch_ /
         std::to_string(commitChecksum);
}

nlohmann::json RepositoryConfig::ToJson() const {
  nlohmann::json j;
  j["repo_name"] = repositoryName_;
  j["repo_folder"] = repositoryFolder_;
  j["current_branch"] = currentBranch_;
  j["branches"] = branches_;
  return j;
}

fs::path RepositoryConfig::FormRepositoryConfigFilePath(
    const std::string& repositoryName) {
  return GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName / REPOSITORY_CONFIG_FILE;
}

void RepositoryConfig::CheckBranchExist(const types::Branch& branch) const {
  if (!branches_.contains(branch)) {
    throw std::invalid_argument(fmt::format("Branch [{}] does not exist", branch));
  }
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
    throw std::invalid_argument("Repository name or folder cannot be empty");
  }

  if (config.repositoryFolder_ != fs::current_path()) {
    throw std::invalid_argument("Repository folders are not equal");
  }

  if (config.branches_.empty()) {
    throw std::invalid_argument("Branches cannot be empty");
  }

  if (config.currentBranch_.empty()) {
    throw std::invalid_argument("No current branch selected");
  }

  if (!config.branches_.contains(config.currentBranch_)) {
    throw std::invalid_argument(
        fmt::format("Branch [{}] set as current does not exist", config.currentBranch_));
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
  std::ofstream file(path);
  if (!file) {
    throw std::runtime_error(fmt::format("Cannot create file '{}'", path.c_str()));
  }
  file << defaultValue;
}

void CreateFolder(const fs::path& path) {
  if (!fs::exists(path) && !fs::create_directories(path)) {
    throw std::runtime_error(
        fmt::format("Cannot create repository folder in '{}'", path.c_str()));
  }
}
