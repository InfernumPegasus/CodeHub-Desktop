#include <iostream>
#include "VersionControlSystem.h"
#include "../validation/Validator.h"
#include "../serializer/JsonSerializer.h"
#include "../filemanager/Configs.h"
#include "../web/WebService.h"

VersionControlSystem::VersionControlSystem() :
        repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.UpdateConfigFile();
}

void VersionControlSystem::CreateRepository(
        std::string repositoryName,
        bool initRepository) {
    std::string repositoryFolder = std::filesystem::current_path().string();
    Validator::ValidateRepositoryName(repositoryName);
    if (!Validator::IsValidRepositoryName(repositoryName)) {
        std::cout << "'" << repositoryName << "' is wrong repository name!\n";
        return;
    }

    if (ExistsByFolder(repositoryFolder)) {
        std::cout << "There is another repo in this folder!\n";
        return;
    }

    if (ExistsByName(repositoryName)) {
        std::cout << "Repository '" << repositoryName << "' already exists!\n";
        return;
    }

    nameFolderMap_.emplace(repositoryName, repositoryFolder);

    Repository repository(repositoryName, repositoryFolder);
    if (initRepository) {
        repository.InitManagers();
    }

    std::cout << "Repository '" << repositoryName
              << "' created in folder '" << repositoryFolder << "'.\n";
}

void VersionControlSystem::CheckStatus() const {
    for (const auto &[name, folder]: nameFolderMap_) {
        if (folder == std::filesystem::current_path()) {
            Repository repository(name, folder);
            repository.InitConfigManager();
            repository.InitIgnoreManager();

            std::cout << "Repository '" <<
                      name << "' status: ";
            if (auto changedFiles = repository.ChangedFiles();
                    changedFiles.empty()) {
                std::cout << "Up-to-date.\n";
            } else {
                for (const auto &[fileName, hash]: changedFiles) {
                    std::cout << "\nfile: '" << fileName;
                }
                std::cout << "\n\nTotal " << changedFiles.size() << " files changed.\n";
            }
        }
    }
}

void VersionControlSystem::DoCommit(std::string_view message) {
    if (message.empty()) {
        std::cout << "Message cannot be empty.\n";
        return;
    }

    auto repository = JsonSerializer::GetRepositoryByFolder(
            std::filesystem::current_path());
    repository.InitManagers();
    repository.DoCommit(message);
}

void VersionControlSystem::DeleteRepository() {
    std::string currentDir = std::filesystem::current_path();
    for (const auto &[name, folder]: nameFolderMap_) {
        if (folder == currentDir) {
            nameFolderMap_.erase(name);
        }
    }
}

void VersionControlSystem::Init() {
    if (!repositoriesManager_.Read() &&
        repositoriesManager_.Create()) {
        repositoriesManager_.Read();
    }
}

VersionControlSystem::NameFolderMap VersionControlSystem::NameAndFolderMap() const {
    return nameFolderMap_;
}

bool VersionControlSystem::ExistsByName(std::string_view repositoryName) const {
    return nameFolderMap_.contains(repositoryName.data());
}

bool VersionControlSystem::ExistsByFolder(std::string_view repositoryFolder) const {
    return std::ranges::any_of(nameFolderMap_.cbegin(), nameFolderMap_.cend(),
                               [&](auto &pair) { return pair.first != repositoryFolder; });
}

void VersionControlSystem::ShowRepositories() const {
    for (const auto &[name, folder]: nameFolderMap_) {
        if (ExistsByName(name) &&
            ExistsByFolder(folder)) {
            std::cout << "'" << name << "' : '" << folder << "'\n";
        }
    }
}

void VersionControlSystem::CommitsLog() {
    auto repository = JsonSerializer::GetRepositoryByFolder(
            std::filesystem::current_path());
    repository.InitCommitsManager();

    for (const auto &commit: repository.Commits()) {
        std::cout << commit << "\n";
    }
}

/*
 * TODO переделать
 */
void VersionControlSystem::Push() {
    auto repository = JsonSerializer::GetRepositoryByFolder(
            std::filesystem::current_path());
    repository.InitCommitsManager();

//    auto response = WebService::PostCommit(repository.Commits().back());
//    std::cout << response.text << "\n";
}

/*
 * TODO доделать
 */
std::vector<Commit> VersionControlSystem::CommitsToPush() {
    std::vector<Commit> commits;
    return commits;
}

