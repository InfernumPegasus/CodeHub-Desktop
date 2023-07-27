#include "config/RepositoryConfig.h"

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

RepositoryConfig RepositoryConfigFromFile(const fs::path& configPath) {
  std::ifstream ifs(configPath);
  if (!ifs) {
    throw std::runtime_error(
        fmt::format("Repositories file '{}' cannot be opened", configPath.c_str()));
  }
  const auto json = nlohmann::json::parse(ifs);
  return {json["repo_name"], json["repo_folder"], json["current_branch"],
          json["branches"]};
}

// TODO implement
void VerifyRepositoryConfig(const RepositoryConfig& config) {}

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
