#include <iostream>
#include "VersionControlSystem.h"
#include "../validation/Validator.h"
#include "../serializer/JsonSerializer.h"
#include "../filemanager/Configs.h"
#include "../web/WebService.h"
#include "../filemanager/RestoreFileManager.h"

VersionControlSystem::VersionControlSystem() :
        repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.Update();
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
        if (folder != fs::current_path()) continue;

        Repository repository(name, folder);
        repository.InitManagers();

        std::cout << "Repository '" << name << "' status: ";
        if (auto changedFiles = repository.ChangedFiles();
                changedFiles.empty() || repository.Commits().empty()) {
            std::cout << "Up-to-date.\n";
        } else {
            for (const auto &[fileName, hash]: changedFiles) {
                std::cout << "\n\tChanged:\t" <<
                          fs::relative(fileName);
            }
            std::cout << "\n";
        }
    }
}

void VersionControlSystem::DoCommit(std::string_view message) {
    if (message.empty()) {
        std::cout << "Commit message cannot be empty.\n";
        return;
    }

    auto repository = JsonSerializer::GetRepositoryByFolder(
            fs::current_path());
    repository.InitManagers();
    repository.DoCommit(message);
}

void VersionControlSystem::DeleteRepository() {
    std::string currentDir = fs::current_path();
    erase_if(nameFolderMap_,
             [&](auto &pair) -> bool { return pair.first == currentDir; });
}

void VersionControlSystem::Init() {
    repositoriesManager_.Init();
}

VersionControlSystem::NameFolderMap VersionControlSystem::NameAndFolderMap() const {
    return nameFolderMap_;
}

bool VersionControlSystem::ExistsByName(
        std::string_view repositoryName) const {
    return nameFolderMap_.contains(repositoryName.data());
}

bool VersionControlSystem::ExistsByFolder(
        std::string_view repositoryFolder) const {
    return std::ranges::any_of(
            nameFolderMap_.cbegin(), nameFolderMap_.cend(),
            [&](auto &pair) { return pair.first == repositoryFolder; }
    );
}

void VersionControlSystem::ShowRepositories() const {
    if (nameFolderMap_.empty()) {
        std::cout << "No repositories yet.\n";
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
    auto repository = JsonSerializer::GetRepositoryByFolder(
            fs::current_path());
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
            fs::current_path());
    repository.InitCommitsManager();

    auto response =
            WebService::PostCommit(repository.Commits().back());
    std::cout << response.text << "\n";
}

/*
 * TODO доделать
 */
std::vector<Commit> VersionControlSystem::CommitsToPush() {
    std::vector<Commit> commits;
    return commits;
}

void VersionControlSystem::RestoreFiles(int32_t checksum) {
    auto repository = JsonSerializer::GetRepositoryByFolder(
            fs::current_path()
    );
    repository.InitCommitsManager();
    repository.InitConfigManager();
    auto commits = repository.Commits();
    for (const auto &item: commits) {
        std::cout << item << "\n";
    }
    auto found = std::find_if(commits.cbegin(),
                              commits.cend(),
                              [checksum](const Commit &c) {
                                  return c.Checksum() == checksum;
                              });
    if (found == commits.cend()) return;

    std::cout << "Commit found.\n";
    Repository::RestoreCommitFiles(checksum);
}

