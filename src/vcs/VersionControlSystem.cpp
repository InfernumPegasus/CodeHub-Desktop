#include <iostream>
#include "VersionControlSystem.h"
#include "../validation/Validator.h"
#include "../serializer/JsonSerializer.h"
#include "../web/WebService.h"
#include "../filemanager/RestoreFileManager.h"

VersionControlSystem::VersionControlSystem() :
        repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.Update();
}

void VersionControlSystem::Init() {
    repositoriesManager_.Init();
    userManager_.Init();
}

bool VersionControlSystem::IsUniqueRepositoryData(
        const std::string &name,
        const std::string &folder) const {
    return Validator::IsValidRepositoryName(name) &&
           !ExistsByFolder(folder) &&
           !ExistsByName(name);
}

bool VersionControlSystem::ExistsByName(
        std::string_view repositoryName) const {
    return nameFolderMap_.contains(repositoryName.data());
}

bool VersionControlSystem::ExistsByFolder(
        std::string_view repositoryFolder) const {
    return std::ranges::any_of(
            nameFolderMap_.cbegin(), nameFolderMap_.cend(),
            [&](auto &pair) {
                return pair.second == repositoryFolder;
            }
    );
}

void VersionControlSystem::CreateRepository(
        std::string repositoryName) {
    const auto repositoryFolder = std::filesystem::current_path();
    Validator::ValidateRepositoryName(repositoryName);
    if (!IsUniqueRepositoryData(repositoryName, repositoryFolder)) {
        std::cout << "Cannot create repository.\n"
                     "Use --repositories command to see all repositories.\n";
        return;
    }

    Repository repository(repositoryName, repositoryFolder);
    repository.InitManagers();
    nameFolderMap_.emplace(repositoryName, repositoryFolder);

    std::cout << "Repository '" << repositoryName
              << "' created in folder '" << repositoryFolder << "'.\n";
}

void VersionControlSystem::CheckStatus() const {
    if (nameFolderMap_.empty()) {
        std::cout << "You have no repositories.\n";
        return;
    }

    std::ranges::find_if(
            nameFolderMap_.cbegin(),
            nameFolderMap_.cend(),
            [&](const auto &pair) -> bool {
                const std::string name = pair.first;
                const std::string folder = pair.second;
                if (folder != fs::current_path()) return false;

                Repository repository(name, folder);
                repository.InitManagers();

                std::cout << "Repository '" << name << "' status: ";
                if (repository.Commits().empty()) {
                    std::cout << "There are no commits yet.\n"
                                 "Use --commit command to do commit.\n";
                } else if (auto changedFiles = repository.ChangedFiles();
                        changedFiles.empty()) {
                    std::cout << "Up-to-date.\n";
                } else {
                    for (const auto &[fileName, hash]: changedFiles) {
                        std::cout << "\n\tChanged:\t" <<
                                  fs::relative(fileName);
                    }
                    std::cout << "\n";
                }
                return true;
            });
}

void VersionControlSystem::DoCommit(std::string_view message) {
    if (message.empty()) {
        std::cout << "Commit message cannot be empty.\n";
        return;
    }
    if (nameFolderMap_.empty()) {
        std::cout << "You have no repositories.\n";
        return;
    }

    auto repository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repository.has_value()) return;

    repository->InitManagers();
    repository->DoCommit(message);
}

void VersionControlSystem::Push() {
    if (nameFolderMap_.empty()) {
        std::cout << "You have no repositories.\n";
        return;
    }

    auto localRepository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!localRepository.has_value()) {
        std::cout << "There is no repository in this folder.\n";
        return;
    }

    auto loginResponse = WebService::PostLogin(
            userManager_.Email(),
            userManager_.Password());
    if (!WebService::NoErrorsInResponseCode(loginResponse.status_code)) {
        std::cout << "Cannot save repository on server 1.\n";
        return;
    }

    const auto foundRepository =
            WebService::GetRepository(localRepository->Name());
    // Repo exists in DB
    if (!foundRepository.has_value()) {
        const auto response =
                WebService::PostRepository(localRepository.value());
        if (!WebService::NoErrorsInResponseCode(response.status_code)) {
            std::cout << "Cannot save repository on server 2.\n";
            return;
        }
    }

    auto foundCommits = foundRepository->Commits();
    // Repo exists locally only
    localRepository->InitCommitsManager();
    const auto localCommits = localRepository->Commits();

    if (localCommits.empty() ||
        localCommits.size() == foundCommits.size()) {
        std::cout << "No commits to push!\n";
        return;
    }

    auto response =
            WebService::PatchRepository(
                    localRepository->Name(),
                    localRepository.value(),
                    false,
                    localCommits);
    if (!WebService::NoErrorsInResponseCode(response.status_code)) {
        std::cout << "Cannot push commits.\n";
    }
}

void VersionControlSystem::ShowRepositories() const {
    if (nameFolderMap_.empty()) {
        std::cout << "You have no repositories.\n";
        return;
    }
    for (const auto &[name, folder]: nameFolderMap_) {
        if (ExistsByName(name) &&
            ExistsByFolder(folder)) {
            std::cout << "'" << name << "' : '" << folder << "'\n";
        }
    }
}

void VersionControlSystem::CommitsLog() {
    if (nameFolderMap_.empty()) {
        std::cout << "You have no repositories.\n";
        return;
    }
    auto repository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repository.has_value()) return;

    repository->InitCommitsManager();

    for (const auto &commit: repository->Commits()) {
        std::cout << commit << "\n";
    }
}

void VersionControlSystem::RestoreFiles(size_t checksum) {
    if (nameFolderMap_.empty()) {
        std::cout << "You have no repositories.\n";
        return;
    }
    auto repository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repository.has_value()) return;

    repository->InitCommitsManager();
    repository->InitConfigManager();
    const auto commits = repository->Commits();
    const auto found = std::find_if(commits.cbegin(),
                                    commits.cend(),
                                    [checksum](const Commit &c) {
                                        return c.Checksum() == checksum;
                                    });
    if (found == commits.cend()) return;
    Repository::RestoreCommitFiles(checksum);
}

// TODO
void VersionControlSystem::Clone(std::string_view repositoryName) {
    if (ExistsByName(repositoryName) ||
        ExistsByFolder(fs::current_path().string())) {
        std::cout << "There is another repo in this folder.\n";
        return;
    }
}

// TODO
void VersionControlSystem::Pull() {
}
