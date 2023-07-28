#include "vcs/VersionControlSystem.h"

#include <iostream>
#include <ranges>
#include <utility>

#include "config/RepositoryConfig.h"
#include "filecomparator/FileComparator.h"
#include "filemanager/RestoreFileManager.h"
#include "json/JsonSerializer.h"
#include "log/Logger.h"
#include "validation/Validator.h"
#include "web/WebService.h"

VersionControlSystem::VersionControlSystem()
    : userManager_(std::make_unique<UserFileManager>()) {}

VersionControlSystem::~VersionControlSystem() { SaveVcsState(); }

void VersionControlSystem::Init() {
  const auto folder = GetHomeDirectory() / VCS_CONFIG_FOLDER;
  if (!fs::exists(folder) && !fs::create_directories(folder)) {
    logging::Log(LOG_WARNING,
                 "Cannot create vcs config folder or it have been already created");
  }

  const auto repositoriesFile = folder / VCS_REPOSITORIES_FILE;

  const auto createDefault = [&repositoriesFile]() {
    std::ofstream file(repositoriesFile);
    if (!file) {
      throw std::runtime_error("Cannot create repositories file");
    }
    nlohmann::json j;
    j["repositories"] = types::NameFolderMap{};
    file << j.dump(2);
  };

  const auto read = [this, &repositoriesFile]() {
    std::ifstream file(repositoriesFile);
    if (!file) {
      throw std::runtime_error("Cannot read repositories file");
    }
    nlohmann::json j = nlohmann::json::parse(file);
    nameFolderMap_ = j["repositories"];
  };

  if (!fs::exists(repositoriesFile) || fs::is_empty(repositoriesFile)) {
    createDefault();
  } else {
    read();
  }
}

void VersionControlSystem::CheckRepositoriesExist() const {
  if (nameFolderMap_.empty()) {
    throw std::runtime_error("You have no repositories.");
  }
}

bool VersionControlSystem::ExistsByName(std::string_view repositoryName) const {
  return nameFolderMap_.contains(repositoryName.data());
}

bool VersionControlSystem::ExistsByFolder(const fs::path& repositoryFolder) const {
  return std::ranges::any_of(
      nameFolderMap_.cbegin(), nameFolderMap_.cend(),
      [&](auto&& pair) { return pair.second == repositoryFolder; });
}

void VersionControlSystem::CreateRepository(std::string repositoryName) {
  const auto repositoryFolder = std::filesystem::current_path();
  Validator::ValidateRepositoryName(repositoryName);
  const auto checkIfUniqueRepoData = [this](auto&& repositoryName,
                                            auto&& repositoryFolder) {
    if (!Validator::IsValidRepositoryName(repositoryName)) {
      throw std::invalid_argument(
          fmt::format("Repository name '{}' is not valid", repositoryName));
    }

    if (ExistsByFolder(repositoryFolder) || ExistsByName(repositoryName)) {
      throw std::runtime_error(
          fmt::format("Repository with name '{}' cannot be created in folder '{}'",
                      repositoryName, repositoryFolder.c_str()));
    }
  };
  checkIfUniqueRepoData(repositoryName, repositoryFolder);

  CreateRepositoryConfigs(repositoryName);

  RepositoryConfig config{
      repositoryName, repositoryFolder, DEFAULT_BRANCH_NAME, {DEFAULT_BRANCH_NAME}};
  CheckRepositoryConfig(config);
  Repository repository(config);
  nameFolderMap_.emplace(repositoryName, repositoryFolder);
}

void VersionControlSystem::CheckStatus() const {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);

  if (!nameFolderMap_.contains(repository.Name())) {
    throw std::runtime_error(
        fmt::format("Found repository '{}', but there is no such repository in app utils",
                    repository.Name()));
  }

  repository.InitManagers();

  const auto printFilesByStatus = [](std::string_view status, const char symbol,
                                     const types::FileHashMap& files) {
    if (!files.empty()) {
      fmt::print("\nThere are {} {} files:", files.size(), status);
      for (const auto& [fileName, hash] : files) {
        fmt::print("\n\t[{}] {}", symbol, fileName.c_str());
      }
      std::cout << std::endl;
    }
  };

  fmt::print("Repository '{}' status: ", repository.Name());
  const auto createdFiles = repository.CreatedFiles();
  if (repository.Commits().empty()) {
    std::cout << "There are no commits yet.\n"
                 "Use --commit command to do commit.\n";
    printFilesByStatus("Created", '+', createdFiles);
  } else {
    const auto changedFiles = repository.ChangedFiles();
    const auto removedFiles = repository.RemovedFiles();

    if (changedFiles.empty() && removedFiles.empty() && createdFiles.empty()) {
      std::cout << "Up-to-date.\n";
      return;
    }

    printFilesByStatus("Created", '+', createdFiles);
    printFilesByStatus("Changed", '~', changedFiles);
    printFilesByStatus("Removed", '-', removedFiles);
  }
}

void VersionControlSystem::DoCommit(const std::string& message) {
  CheckRepositoriesExist();
  if (message.empty()) {
    throw std::runtime_error("Commit message cannot be empty");
  }
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);
  repository.InitManagers();
  repository.DoCommit(message);
}

void VersionControlSystem::Push() {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository localRepository(config);

  const auto loginResponse =
      WebService::PostLogin(userManager_->Email(), userManager_->Password());
  if (!WebService::NoErrorsInResponse(loginResponse.status_code)) {
    throw std::runtime_error("Cannot save repository on server.");
  }

  const auto foundRepository = WebService::GetRepository(localRepository.Name());
  // Repo exists in DB
  if (!foundRepository.has_value()) {
    const auto response = WebService::PostRepository(localRepository);
    if (!WebService::NoErrorsInResponse(response.status_code)) {
      throw std::runtime_error("Cannot save repository on server.");
    }
  }

  const auto foundCommits = foundRepository->Commits();
  // Repo exists locally only
  const auto& localCommits = localRepository.Commits();

  if (localCommits.empty() || localCommits.size() == foundCommits.size()) {
    std::cout << "No commits to push!\n";
    return;
  }

  const auto response = WebService::PatchRepository(localRepository.Name(),
                                                    localRepository, false, localCommits);
  if (!WebService::NoErrorsInResponse(response.status_code)) {
    throw std::runtime_error("Cannot push commits.");
  }
}

void VersionControlSystem::ShowRepositories() const {
  CheckRepositoriesExist();
  for (const auto& [name, folder] : nameFolderMap_) {
    if (ExistsByName(name) && ExistsByFolder(folder)) {
      fmt::print("'{}' : '{}'\n", name, folder.c_str());
    }
  }
}

void VersionControlSystem::CommitsLog() const {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);
  repository.InitManagers();

  const auto commits = repository.Commits();

  if (commits.empty()) {
    throw std::runtime_error("You have no commits yet.");
  }

  for (const auto& commit : commits) {
    std::cout << commit << "\n";
  }
}

void VersionControlSystem::ShowFileDifference(const fs::path& filename) {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);
  repository.InitManagers();

  const auto commits = repository.Commits();
  if (commits.empty()) {
    throw std::runtime_error(
        fmt::format("No commits in repository '{}'", repository.Name()));
  }

  const auto findCommitWithFile = [&commits](auto&& file) {
    for (const auto& commit : std::ranges::reverse_view(commits)) {
      const auto& files = commit.Files();
      const auto found = std::find_if(files.begin(), files.end(),
                                      [&file](auto&& f) { return f.Name() == file; });
      if (found != files.cend() && (found->Status() != FileStatus::Deleted &&
                                    found->Status() != FileStatus::Unknown)) {
        return commit;
      }
    }
    throw std::runtime_error(
        fmt::format("There is no file '{}' in commits", file.c_str()));
  };
  const auto commit = findCommitWithFile(filename);

  const auto file = GetHomeDirectory() / VCS_CONFIG_FOLDER / repository.Name() /
                    repository.CurrentBranch() / std::to_string(commit.Checksum()) /
                    filename;

  const auto difference = FileComparator::Compare(file, filename);
  if (difference.empty()) {
    fmt::print("File '{}' hasn't changed from the past version\n", filename.c_str());
  } else {
    for (const auto& [lineNumber, lines] : difference) {
      fmt::print("[{}]:\n\t{}\n\t{}\n", lineNumber, lines.first, lines.second);
    }
  }
}

void VersionControlSystem::RestoreFiles(size_t checksum) {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);
  repository.InitManagers();

  const auto& commits = repository.Commits();
  const auto found =
      std::find_if(commits.cbegin(), commits.cend(),
                   [checksum](const Commit& c) { return c.Checksum() == checksum; });
  if (found == commits.cend()) return;

  RestoreFileManager::CopyFolderRecursive(
      GetHomeDirectory() / VCS_CONFIG_FOLDER / repository.Name() /
          repository.CurrentBranch() / std::to_string(checksum),
      fs::current_path());
}

void VersionControlSystem::CreateBranch(std::string name) {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);

  repository.ChangeBranch(std::move(name));
}

void VersionControlSystem::ShowBranches() const {
  CheckRepositoriesExist();
  const auto config =
      ReadRepositoryConfigFromFile(RepositoryConfig::FormRepositoryFolderPath(
          GetRepositoryNameByFolder(fs::current_path())));
  CheckRepositoryConfig(config);
  Repository repository(config);
  repository.InitManagers();

  const auto& branches = repository.Branches();
  if (branches.empty()) {
    throw std::runtime_error("Cannot find any branches");
  }
  const auto& currentBranch = repository.CurrentBranch();

  if (!branches.contains(currentBranch)) {
    throw std::runtime_error(fmt::format("Repository {} does not contain branch {}",
                                         repository.Name(), currentBranch));
  }

  for (const auto& branch : branches) {
    fmt::print("{} {}\n", branch, (branch == currentBranch) ? "[current]" : "");
  }
}

std::string VersionControlSystem::GetRepositoryNameByFolder(
    const fs::path& folder) const {
  const auto found =
      std::find_if(nameFolderMap_.cbegin(), nameFolderMap_.cend(),
                   [&folder](auto&& pair) { return pair.second == folder; });
  if (found == nameFolderMap_.cend()) {
    throw std::invalid_argument(
        fmt::format("There is no repository in folder '{}'", folder.c_str()));
  }
  return found->first;
}

void VersionControlSystem::CreateRepositoryConfigs(const std::string& repositoryName) {
  const auto createRepositoryFolder = [&repositoryName]() {
    const auto folder =
        GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName / DEFAULT_BRANCH_NAME;
    // Creating repo folder with default branch name called "master"
    if (!fs::exists(folder) && !fs::create_directories(folder)) {
      throw std::runtime_error(
          fmt::format("Cannot create repository folder in '{}'", folder.c_str()));
    }
  };
  createRepositoryFolder();

  const auto updateConfigFile = [](auto&& path, auto&& defaultValue) {
    if (!fs::exists(path)) {
      std::ofstream commitsFile(path);
      if (!commitsFile) {
        throw std::runtime_error(fmt::format("Cannot create file '{}'", path.c_str()));
      }
      commitsFile << defaultValue;
    }
  };

  const auto commitsJson = JsonSerializer::CommitsToJson(types::Commits{});
  updateConfigFile(GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName /
                       DEFAULT_BRANCH_NAME / COMMITS_FILE,
                   commitsJson.dump(2));

  nlohmann::json trackedJson;
  trackedJson["tracked_files"] = types::FileHashMap{};
  updateConfigFile(GetHomeDirectory() / VCS_CONFIG_FOLDER / repositoryName /
                       DEFAULT_BRANCH_NAME / TRACKED_FILE,
                   trackedJson.dump(2));
}

void VersionControlSystem::SaveVcsState() const {
  std::ofstream ofs(GetHomeDirectory() / VCS_CONFIG_FOLDER / VCS_REPOSITORIES_FILE);
  nlohmann::json j;
  j["repositories"] = nameFolderMap_;
  ofs << j.dump(2);
}
