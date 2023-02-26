#ifndef CODEHUB_VERSIONCONTROLSYSTEM_H
#define CODEHUB_VERSIONCONTROLSYSTEM_H

#include "repository/Repository.h"
#include "filemanager/RepositoriesManager.h"

class VersionControlSystem {
public:
    using NameFolderMap = std::unordered_map<std::string, std::string>;

public:
    ~VersionControlSystem();

public:
    void CreateRepository(std::string repositoryName);

    void DeleteRepository(std::string_view repositoryName);

    std::set<Repository>::iterator FindRepositoryByName(
            std::string_view name);

    void CheckStatus(std::string_view repoName);

public:
    NameFolderMap NameAndFolderMap() const;

public:
    void Init();

//public:
//    void PushToServer(const Repository &repository);

private:
    std::set<Repository> repositories_;

    RepositoriesManager repositoriesManager_;
};


#endif //CODEHUB_VERSIONCONTROLSYSTEM_H
