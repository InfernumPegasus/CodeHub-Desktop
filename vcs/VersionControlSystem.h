#ifndef CODEHUB_VERSIONCONTROLSYSTEM_H
#define CODEHUB_VERSIONCONTROLSYSTEM_H

#include <optional>
#include "../repository/Repository.h"
#include "../filemanager/RepositoriesManager.h"

using NameFolderMap = std::unordered_map<std::string, std::string>;

class VersionControlSystem {
public:
    VersionControlSystem();

    ~VersionControlSystem();

public:
    void CreateRepository(std::string repositoryName,
                          bool initRepository);

private:
    bool IsUniqueRepositoryData(
            const std::string &name,
            const std::string &folder) const;

public:
    void CheckStatus() const;

    static void DoCommit(std::string_view message);

    void DeleteRepository();

    bool ExistsByName(std::string_view repositoryName) const;

    bool ExistsByFolder(std::string_view repositoryFolder) const;

    void ShowRepositories() const;

    static void CommitsLog();

    static void Push();

    static std::optional<std::vector<Commit>> CommitsToPush();

    static std::vector<Commit> CommitsDifference(const std::vector<Commit> &vec1,
                                                 const std::vector<Commit> &vec2);

public:
    const NameFolderMap &NameAndFolderMap() const;

public:
    void Init();

public:
    static void RestoreFiles(int32_t checksum);

private:
    NameFolderMap nameFolderMap_;

    RepositoriesManager repositoriesManager_;
};


#endif //CODEHUB_VERSIONCONTROLSYSTEM_H
