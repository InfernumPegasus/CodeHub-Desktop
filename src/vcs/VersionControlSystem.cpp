#include "vcs/VersionControlSystem.h"

#include <iostream>

#include "filecomparator/FileComparator.h"
#include "filemanager/RestoreFileManager.h"
#include "serializer/JsonSerializer.h"
#include "validation/Validator.h"
#include "web/WebService.h"

VersionControlSystem::VersionControlSystem() : repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() { repositoriesManager_.Update(); }

void VersionControlSystem::Init() {
  repositoriesManager_.Init();
  userManager_.Init();
}

bool VersionControlSystem::IsUniqueRepositoryData(const std::string& name,
                                                  const fs::path& folder) const {
  return Validator::IsValidRepositoryName(name) && !ExistsByFolder(folder) &&
         !ExistsByName(name);
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
  return std::ranges::any_of(nameFolderMap_.cbegin(), nameFolderMap_.cend(),
                             [&](auto& pair) { return pair.second == repositoryFolder; });
}

void VersionControlSystem::CreateRepository(std::string repositoryName) {
  const auto repositoryFolder = std::filesystem::current_path();
  Validator::ValidateRepositoryName(repositoryName);
  if (!IsUniqueRepositoryData(repositoryName, repositoryFolder)) {
    std::cout << "Cannot create repository.\n"
                 "Use --repositories command to see all repositories.\n";
    return;
  }

  // TODO add branch determination
  Repository repository(repositoryName, repositoryFolder, "master");
  repository.InitManagers();
  nameFolderMap_.emplace(repositoryName, repositoryFolder);

  std::cout << "Repository '" << repositoryName << "' created in folder "
            << repositoryFolder << ".\n";
}

void VersionControlSystem::CheckStatus() const {
  CheckRepositoriesExist();
  const auto folder = fs::current_path();
  auto repository = JsonSerializer::GetRepositoryByFolder(folder);
  if (!nameFolderMap_.contains(repository->Name()) || !repository.has_value()) {
    throw std::runtime_error("Cannot open repository.");
  }

  repository->InitManagers();

  const auto printFilesByStatus = [](std::string_view status, const char symbol,
                                     const types::FileHashMap& files) {
    const auto size = files.size();
    if (!files.empty()) {
      std::cout << "\nYou have " << size << " " << status << " files:";
      for (const auto& [fileName, hash] : files) {
        std::cout << "\n\t[" << symbol << "] " << fileName;
      }
      std::cout << std::endl;
    }
  };

  std::cout << "Repository '" << repository->Name() << "' status: ";
  if (repository->Commits().empty()) {
    std::cout << "There are no commits yet.\n"
                 "Use --commit command to do commit.\n";
    printFilesByStatus("Created", '+', repository->CreatedFiles());
  } else {
    const auto createdFiles = repository->CreatedFiles();
    const auto changedFiles = repository->ChangedFiles();
    const auto removedFiles = repository->RemovedFiles();

    if (changedFiles.empty() && removedFiles.empty() && createdFiles.empty()) {
      std::cout << "Up-to-date.\n";
      return;
    }

    printFilesByStatus("Created", '+', createdFiles);
    printFilesByStatus("Changed", '~', changedFiles);
    printFilesByStatus("Removed", '-', removedFiles);
  }
}

void VersionControlSystem::DoCommit(std::string_view message) {
  CheckRepositoriesExist();
  if (message.empty()) {
    std::cout << "Commit message cannot be empty.\n";
    return;
  }

  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  if (!repository.has_value()) return;

  repository->InitManagers();
  repository->DoCommit(message);
}

void VersionControlSystem::Push() {
  CheckRepositoriesExist();
  auto localRepository = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  if (!localRepository.has_value()) {
    std::cout << "There is no repository in this folder.\n";
    return;
  }

  const auto loginResponse =
      WebService::PostLogin(userManager_.Email(), userManager_.Password());
  if (!WebService::NoErrorsInResponse(loginResponse.status_code)) {
    std::cout << "Cannot save repository on server!\n";
    return;
  }

  const auto foundRepository = WebService::GetRepository(localRepository->Name());
  // Repo exists in DB
  if (!foundRepository.has_value()) {
    const auto response = WebService::PostRepository(localRepository.value());
    if (!WebService::NoErrorsInResponse(response.status_code)) {
      std::cout << "Cannot save repository on server!\n";
      return;
    }
  }

  const auto foundCommits = foundRepository->Commits();
  // Repo exists locally only
  localRepository->InitCommitsManager();
  const auto localCommits = localRepository->Commits();

  if (localCommits.empty() || localCommits.size() == foundCommits.size()) {
    std::cout << "No commits to push!\n";
    return;
  }

  const auto response = WebService::PatchRepository(
      localRepository->Name(), localRepository.value(), false, localCommits);
  if (!WebService::NoErrorsInResponse(response.status_code)) {
    std::cout << "Cannot push commits.\n";
  }
}

void VersionControlSystem::ShowRepositories() const {
  CheckRepositoriesExist();
  for (const auto& [name, folder] : nameFolderMap_) {
    if (ExistsByName(name) && ExistsByFolder(folder)) {
      std::cout << "'" << name << "' : '" << folder << "'\n";
    }
  }
}

void VersionControlSystem::CommitsLog() const {
  CheckRepositoriesExist();
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  if (!repository.has_value()) return;

  repository->InitCommitsManager();
  const auto commits = repository->Commits();

  if (commits.empty()) {
    std::cout << "You have no commits yet.\n";
    return;
  }
  for (const auto& commit : commits) {
    std::cout << commit << "\n";
  }
}

void VersionControlSystem::ShowFileDifference(std::string_view filename) {
  CheckRepositoriesExist();
  auto repo = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  if (!repo.has_value()) {
    std::cout << "No repository in this folder.\n";
    return;
  }

  repo->InitCommitsManager();
  if (repo->Commits().empty()) {
    return;
  }

  const auto file = fs::path{repo->Folder()} / CONFIG_DIRECTORY /
                    std::to_string(repo->Commits().back().Checksum()) / filename;

  const auto difference = FileComparator::Compare(file, filename);
  for (const auto& [lineNumber, lines] : difference) {
    printf("[%lu]:\n\t%s\n\t%s\n", lineNumber, lines.first.c_str(), lines.second.c_str());
  }
}

void VersionControlSystem::RestoreFiles(size_t checksum) {
  CheckRepositoriesExist();
  auto repository = JsonSerializer::GetRepositoryByFolder(fs::current_path());
  if (!repository.has_value()) return;

  repository->InitConfigManager();
  repository->InitCommitsManager();
  const auto commits = repository->Commits();
  const auto found =
      std::find_if(commits.cbegin(), commits.cend(),
                   [checksum](const Commit& c) { return c.Checksum() == checksum; });
  if (found == commits.cend()) return;
  Repository::RestoreCommitFiles(checksum);
}
