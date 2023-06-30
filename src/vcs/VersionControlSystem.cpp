#include <iostream>
#include "vcs/VersionControlSystem.h"
#include "validation/Validator.h"
#include "serializer/JsonSerializer.h"
#include "web/WebService.h"
#include "filemanager/RestoreFileManager.h"
#include "filecomparator/FileComparator.h"

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

void VersionControlSystem::CheckRepositoriesExist() const {
    if (nameFolderMap_.empty()) {
        throw std::runtime_error("You have no repositories.");
    }
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
              << "' created in folder " << repositoryFolder << ".\n";
}

void VersionControlSystem::CheckStatus() const {
    CheckRepositoriesExist();
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
    CheckRepositoriesExist();
    if (message.empty()) {
        std::cout << "Commit message cannot be empty.\n";
        return;
    }

    auto repository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repository.has_value()) return;

    repository->InitManagers();
    repository->DoCommit(message);
}

void VersionControlSystem::Push() {
    CheckRepositoriesExist();

    auto localRepository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!localRepository.has_value()) {
        std::cout << "There is no repository in this folder.\n";
        return;
    }

    auto loginResponse = WebService::PostLogin(
            userManager_.Email(),
            userManager_.Password());
    if (!WebService::NoErrorsInResponse(loginResponse.status_code)) {
        std::cout << "Cannot save repository on server!\n";
        return;
    }

    const auto foundRepository =
            WebService::GetRepository(localRepository->Name());
    // Repo exists in DB
    if (!foundRepository.has_value()) {
        const auto response =
                WebService::PostRepository(localRepository.value());
        if (!WebService::NoErrorsInResponse(response.status_code)) {
            std::cout << "Cannot save repository on server!\n";
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
    if (!WebService::NoErrorsInResponse(response.status_code)) {
        std::cout << "Cannot push commits.\n";
    }
}

void VersionControlSystem::ShowRepositories() const {
    CheckRepositoriesExist();
    for (const auto &[name, folder]: nameFolderMap_) {
        if (ExistsByName(name) &&
            ExistsByFolder(folder)) {
            std::cout << "'" << name << "' : '" << folder << "'\n";
        }
    }
}

void VersionControlSystem::CommitsLog() const {
    CheckRepositoriesExist();
    auto repository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repository.has_value()) return;

    repository->InitCommitsManager();
    const auto commits = repository->Commits();

    if (commits.empty()) {
        std::cout << "You have no commits yet.\n";
        return;
    }
    for (const auto &commit: commits) {
        std::cout << commit << "\n";
    }
}

void VersionControlSystem::ShowFileDifference(std::string_view filename) {
    CheckRepositoriesExist();
    auto repo =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repo.has_value()) {
        std::cout << "No repository in this folder.\n";
        return;
    }

    if (repo.has_value()) {
        repo->InitCommitsManager();

        if (repo->Commits().empty()) {
            return;
        }

        const auto file =
                fs::path{repo->Folder()} /
                CONFIG_DIRECTORY /
                std::to_string(repo->Commits().back().Checksum()) /
                filename;

        const auto difference = FileComparator::Compare(file, filename);
        for (const auto &[lineNumber, lines]: difference) {
            printf("[%u]:\n\t%s\n\t%s\n",
                   lineNumber, lines.first.c_str(), lines.second.c_str());
        }
    }
}

void VersionControlSystem::RestoreFiles(size_t checksum) {
    CheckRepositoriesExist();
    auto repository =
            JsonSerializer::GetRepositoryByFolder(fs::current_path());
    if (!repository.has_value()) return;

    repository->InitConfigManager();
    repository->InitCommitsManager();
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
