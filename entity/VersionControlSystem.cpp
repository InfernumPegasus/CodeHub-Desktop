#include "VersionControlSystem.h"
#include "validation/Validator.h"

VersionControlSystem::~VersionControlSystem() {
    repositoriesManager_.UpdateConfigFile();
}

void VersionControlSystem::CreateRepository(
        std::string repositoryName) {
    std::string currentDir = absolute(std::filesystem::current_path());
    Validator::Trim(repositoryName);
    Validator::ReplaceWith(repositoryName, " ", "_");
    if (!Validator::IsValidRepositoryName(repositoryName)) {
        std::cout << "'" << repositoryName << "' is wrong repository name!\n";
        return;
    }

    if (FindRepositoryByName(repositoryName) != repositories_.end()) {
        Repository repository(repositoryName, currentDir);
//    repository.Init();
        repositories_.insert(repository);

        repositoriesManager_.AddRepository(repositoryName, currentDir);
        repositoriesManager_.UpdateConfigFile();
    } else {
        std::cout << "Repository '" << repositoryName << "' already exists!\n";
    }
}

void VersionControlSystem::DeleteRepository(
        std::string_view repositoryName) {
    auto pos = FindRepositoryByName(repositoryName);
    if (pos != repositories_.end()) {
        repositories_.erase(pos);
    }
}

std::set<Repository>::iterator VersionControlSystem::FindRepositoryByName(
        std::string_view name) {
    return std::find_if(
            repositories_.begin(),
            repositories_.end(),
            [name](const Repository &repository) { return repository.Name() == name; });
}

void VersionControlSystem::CheckStatus(std::string_view repoName) {
    if (auto repo = FindRepositoryByName(repoName);
            repo != repositories_.end()) {
        std::cout << "Repository [" <<
                  repo->Name() << "] status: ";
        if (auto filesAmount = repo->ChangedFilesAmount();
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
    return repositoriesManager_.NameAndFolderMap();
}

