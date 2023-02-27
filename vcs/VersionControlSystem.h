#ifndef CODEHUB_VERSIONCONTROLSYSTEM_H
#define CODEHUB_VERSIONCONTROLSYSTEM_H

#include "../repository/Repository.h"
#include "../filemanager/RepositoriesManager.h"

class VersionControlSystem {
public:
    using NameFolderMap = std::unordered_map<std::string, std::string>;

public:
    VersionControlSystem();

    ~VersionControlSystem();

public:
    void CreateRepository(const std::string &repositoryName,
                          bool initRepository);

    void CreateRepository(std::string repositoryName,
                          const std::string &repositoryFolder,
                          bool initRepository);

    void DeleteRepository(std::string_view repositoryName);

    void CheckStatus(std::string_view repoName);

    bool Contains(std::string_view repositoryName) const;

public:
    NameFolderMap NameAndFolderMap() const;

public:
    void Init();

//public:
//    void PushToServer(const Repository &repository);

private:
    NameFolderMap nameFolderMap_;

    RepositoriesManager repositoriesManager_;
};


#endif //CODEHUB_VERSIONCONTROLSYSTEM_H
