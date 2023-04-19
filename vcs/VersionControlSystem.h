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
    void CreateRepository(std::string repositoryName,
                          bool initRepository);

    void CheckStatus() const;

    static void DoCommit(std::string_view message);

    void DeleteRepository();

    bool ExistsByName(std::string_view repositoryName) const;

    bool ExistsByFolder(std::string_view repositoryFolder) const;

    void ShowRepositories() const;

    static void CommitsLog();

    static void Push();

    static std::vector<Commit> CommitsToPush();

public:
    NameFolderMap NameAndFolderMap() const;

public:
    void Init();

public:
    static void RestoreFiles(int32_t checksum);

private:
    NameFolderMap nameFolderMap_;

    RepositoriesManager repositoriesManager_;
};


#endif //CODEHUB_VERSIONCONTROLSYSTEM_H
