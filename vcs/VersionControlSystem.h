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

private:
    bool IsUniqueRepositoryData(
            const std::string &name,
            const std::string &folder) const;

public:
    bool ExistsByName(std::string_view repositoryName) const;

    bool ExistsByFolder(std::string_view repositoryFolder) const;

public:
    void CreateRepository(std::string repositoryName);

public:
    void CheckStatus() const;

    void DoCommit(std::string_view message);

    void Push();

    static std::optional<std::vector<Commit>> CommitsToPush();

    static std::vector<Commit> CommitsDifference(const std::vector<Commit> &vec1,
                                                 const std::vector<Commit> &vec2);

    void ShowRepositories() const;

    void CommitsLog();

public:
    void Init();

public:
    void RestoreFiles(size_t checksum);

public:
    void Clone(std::string_view repositoryName);

    void Pull();

private:
    NameFolderMap nameFolderMap_;

    RepositoriesManager repositoriesManager_;
};


#endif //CODEHUB_VERSIONCONTROLSYSTEM_H
