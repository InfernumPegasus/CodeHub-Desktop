#include "VersionControlSystem.h"
#include "../validation/Validator.h"

VersionControlSystem::VersionControlSystem() :
        repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.UpdateConfigFile();
}

void VersionControlSystem::CreateRepository(
        const std::string &repositoryName,
        bool initRepository) {
    CreateRepository(repositoryName,
                     absolute(std::filesystem::current_path()),
                     initRepository);
}

void VersionControlSystem::CreateRepository(
        std::string repositoryName,
        const std::string &repositoryFolder,
        bool initRepository) {
    Validator::Trim(repositoryName);
    Validator::ReplaceWith(repositoryName, " ", "_");
    if (!Validator::IsValidRepositoryName(repositoryName)) {
        std::cout << "'" << repositoryName << "' is wrong repository name!\n";
        return;
    }

    if (nameFolderMap_.find(repositoryName) == nameFolderMap_.end()) {
        nameFolderMap_.emplace(repositoryName, repositoryFolder);

        Repository repository(repositoryName, repositoryFolder);
        if (initRepository) {
            repository.InitManagers();
        }

    } else {
        std::cout << "Repository '" << repositoryName << "' already exists!\n";
    }
}

void VersionControlSystem::DeleteRepository(
        std::string_view repositoryName) {
}

void VersionControlSystem::CheckStatus(std::string_view repoName) {
    if (auto pos = nameFolderMap_.find(repoName.data());
            pos != nameFolderMap_.end()) {
        Repository repository(pos->first, pos->second);
        repository.InitConfigManager();
        repository.InitIgnoreManager();

        std::cout << "Repository [" <<
                  repository.Name() << "] status: ";
        if (auto filesAmount = repository.ChangedFilesAmount();
                filesAmount == 0) {
            std::cout << "Up-to-date.\n";
        } else {
            std::cout << filesAmount << " files changed.\n";
        }
    }
}

void VersionControlSystem::Init() {
    if (repositoriesManager_.ReadConfigFile()) {
        std::cout << "Repository config loaded!\n";
    } else if (repositoriesManager_.CreateConfigFile()) {
        std::cout << "Repository config file created!\n";
    }
}

VersionControlSystem::NameFolderMap VersionControlSystem::NameAndFolderMap() const {
    return nameFolderMap_;
}

bool VersionControlSystem::Contains(std::string_view repositoryName) const {
    return nameFolderMap_.contains(repositoryName.data());
}

