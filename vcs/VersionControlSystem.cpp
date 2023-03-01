#include "VersionControlSystem.h"
#include "../validation/Validator.h"
#include "../serializer/JsonSerializer.h"
#include "../filemanager/Configs.h"

VersionControlSystem::VersionControlSystem() :
        repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.UpdateConfigFile();
    // TODO сделать апдейт коммитов
}

void VersionControlSystem::CreateRepository(
        std::string repositoryName,
        bool initRepository) {
    std::string repositoryFolder = std::filesystem::current_path().string();
    Validator::Trim(repositoryName);
    Validator::ReplaceWith(repositoryName, " ", "_");
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

    std::cout << "Repository '" << repositoryName << "' created in folder '" << repositoryFolder << "'.\n";
}

void VersionControlSystem::CheckStatus() const {
    for (const auto &[name, folder]: nameFolderMap_) {
        if (folder == std::filesystem::current_path()) {
            Repository repository(name, folder);
            std::cout << "Repo created\n";
            repository.InitConfigManager();
            std::cout << "InitConfig\n";
            repository.InitIgnoreManager();
            std::cout << "InitIgnore\n";

            std::cout << "Repository '" <<
                      name << "' status: ";
            if (auto filesAmount = repository.ChangedFilesAmount();
                    filesAmount == 0) {
                std::cout << "Up-to-date.\n";
            } else {
                std::cout << filesAmount << " files changed.\n";
            }
        }
    }
}

void VersionControlSystem::AddFiles(
        const std::vector<std::string> &files) {
    auto repository = JsonSerializer::GetRepositoryByFolder(
            std::filesystem::current_path());
    repository.AddFiles(files);
}


void VersionControlSystem::DoCommit(std::string_view message) {
    auto repository = JsonSerializer::GetRepositoryByFolder(
            std::filesystem::current_path());
    repository.InitManagers();
    repository.DoCommit(message);
}

void VersionControlSystem::DeleteRepository(
        std::string_view repositoryName) {
}

void VersionControlSystem::Init() {
    if (!repositoriesManager_.ReadConfigFile() &&
        repositoriesManager_.CreateConfigFile()) {
        repositoriesManager_.ReadConfigFile();
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
