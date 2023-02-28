#include "VersionControlSystem.h"
#include "../validation/Validator.h"
#include "../serializer/JsonSerializer.h"

VersionControlSystem::VersionControlSystem() :
        repositoriesManager_(&nameFolderMap_) {}

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.UpdateConfigFile();
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

    std::cout << repositoryFolder << std::endl;

    if (ExistsByFolder(repositoryFolder)) {
        std::cout << "There is another repo in this folder!\n";
        return;
    }

    if (ExistsByName(repositoryName)) {
        std::cout << "Repository '" << repositoryName << "' already exists!\n";
        return;
    }

    std::cout << "Repository '" << repositoryName << "' created in folder '" << repositoryFolder << "'.\n";
    nameFolderMap_.emplace(repositoryName, repositoryFolder);

    Repository repository(repositoryName, repositoryFolder);
    if (initRepository) {
        repository.InitManagers();
    }
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
    if (repositoriesManager_.ReadConfigFile()) {
        std::cout << "Repository config loaded!\n";
    } else if (repositoriesManager_.CreateConfigFile()) {
        std::cout << "Repository config file created!\n";
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

