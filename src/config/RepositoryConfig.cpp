#include "config/RepositoryConfig.h"

#include <fmt/ranges.h>

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

RepositoryConfig ReadRepositoryConfig() {
  const auto repositoriesFile =
      GetHomeDirectory() / VCS_CONFIG_FOLDER / VCS_REPOSITORIES_FILE;
  std::ifstream ifs(repositoriesFile);
  if (!ifs) {
    throw std::runtime_error(
        fmt::format("Repositories file '{}' cannot be opened", repositoriesFile.c_str()));
  }
  nlohmann::json j = nlohmann::json::parse(ifs);
  return {j["repo_name"], j["repo_folder"], j["current_branch"], j["branches"]};
}

// TODO implement
void VerifyRepositoryConfig(const RepositoryConfig& config) {}
