#include "vcs/VersionControlSystem.h"

#include <iostream>
#include <utility>

#include "filecomparator/FileComparator.h"
#include "filemanager/RestoreFileManager.h"
#include "log/Logger.h"
#include "serializer/JsonSerializer.h"
#include "validation/Validator.h"
#include "web/WebService.h"

VersionControlSystem::VersionControlSystem()
    : userManager_(std::make_unique<UserFileManager>()) {}

VersionControlSystem::~VersionControlSystem() { /*repositoriesManager_->Update();*/
}

void VersionControlSystem::Init() {
  //  repositoriesManager_->Init();
  //  repositoriesManager_->DeleteIncorrectRepositories();
  //  userManager_->Init();

  const auto createAppConfigFolder = []() {
    const auto folder = GetHomeDirectory() / VCS_CONFIG_FOLDER;
    if (!fs::exists(folder) && !fs::create_directories(folder)) {
      throw std::runtime_error("Cannot create vcs config folder");
    }
    const auto repositoriesFile = folder / VCS_REPOSITORIES_FILE;
    if (!fs::exists(repositoriesFile)) {
      logging::Log(LOG_NOTICE, fmt::format("Trying to create repositories file at '{}'",
                                           repositoriesFile.c_str()));
      std::ofstream file(repositoriesFile);
      if (!file) {
        throw std::runtime_error("Cannot create repositories file");
      }
      if (fs::is_empty(repositoriesFile)) {
        // TODO extract method
        nlohmann::json j;
        j["repositories"] = types::NameFolderMap{};
        file << j.dump(2);
      }
    }
  };
  createAppConfigFolder();
  logging::Log(LOG_NOTICE, "VersionControlSystem::Init success");
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

  const auto createRepositoryFolder = []() {};

  Repository repository(repositoryName, repositoryFolder, "master");
  repository.InitManagers();
  nameFolderMap_.emplace(repositoryName, repositoryFolder);

  logging::Log(LOG_NOTICE, fmt::format("Repository '{}' created in folder '{}'",
                                       repositoryName, repositoryFolder.c_str()));
}

void VersionControlSystem::CheckStatus() const {
  CheckRepositoriesExist();
  const auto folder = fs::current_path();
  auto repository = JsonSerializer::GetRepositoryByFolder(folder);
  if (!nameFolderMap_.contains(repository.Name())) {
    throw std::runtime_error(
        fmt::format("Found repository '{}', but there is no such repository in app utils",
                    repository.Name()));
  }

  repository.InitManagers();

  const auto printFilesByStatus = [](std::string_view status, const char symbol,
                                     const types::FileHashMap& files) {
    const auto size = files.size();
    if (!files.empty()) {
      fmt::print("\nThere are {} {} files:", size, status);
      for (const auto& [fileName, hash] : files) {
        fmt::print("\n\t[{}] {}", symbol, fileName.c_str());
      }
      std::cout << std::endl;
    }
  };

  fmt::print("Repository '{}' status: ", repository.Name());
  if (repository.Commits().empty()) {
    std::cout << "There are no commits yet.\n"
                 "Use --commit command to do commit.\n";
    printFilesByStatus("Created", '+', repository.CreatedFiles());
  } else {
    const auto createdFiles = repository.CreatedFiles();
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
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());

  repository.InitManagers();
  repository.DoCommit(message);
}

void VersionControlSystem::Push() {
  CheckRepositoriesExist();
  auto localRepository = JsonSerializer::GetRepositoryByFolder(fs::current_path());

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
  localRepository.InitCommitsManager();
  const auto localCommits = localRepository.Commits();

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
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  repository.InitCommitsManager();

  const auto commits = repository.Commits();

  if (commits.empty()) {
    throw std::runtime_error("You have no commits yet.");
  }

  for (const auto& commit : commits) {
    std::cout << commit << "\n";
  }
}

void VersionControlSystem::ShowFileDifference(std::string_view filename) {
  CheckRepositoriesExist();
  auto repo = JsonSerializer::GetRepositoryByFolder(fs::current_path());

  repo.InitCommitsManager();
  if (repo.Commits().empty()) {
    throw std::runtime_error(fmt::format("No commits in {} repository.", repo.Name()));
  }

  // TODO search in all commits
  const auto file = GetHomeDirectory() / VCS_CONFIG_FOLDER / repo.Name() /
                    repo.CurrentBranch() /
                    std::to_string(repo.Commits().back().Checksum()) / filename;

  const auto difference = FileComparator::Compare(file, filename);
  for (const auto& [lineNumber, lines] : difference) {
    fmt::print("[{}]:\n\t{}\n\t{}\n", lineNumber, lines.first, lines.second);
  }
}

void VersionControlSystem::RestoreFiles(size_t checksum) {
  CheckRepositoriesExist();
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  repository.InitConfigManager();
  repository.InitCommitsManager();

  const auto commits = repository.Commits();
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
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());

  repository.InitConfigManager();
  repository.InitCommitsManager();
  repository.ChangeBranch(std::move(name));
}

void VersionControlSystem::ShowBranches() const {
  CheckRepositoriesExist();
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());

  repository.InitConfigManager();
  repository.InitBranchesManager();

  const auto branches = repository.Branches();
  if (branches.empty()) {
    throw std::runtime_error("Cannot find any branches");
  }
  const auto currentBranch = repository.CurrentBranch();

  if (!branches.contains(currentBranch)) {
    throw std::runtime_error(fmt::format("Repository {} does not contain branch {}",
                                         repository.Name(), currentBranch));
  }

  for (const auto& branch : branches) {
    fmt::print("{} {}\n", branch, (branch == currentBranch) ? "[current]" : "");
  }
}
