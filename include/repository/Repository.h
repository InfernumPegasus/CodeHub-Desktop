#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include <unordered_map>
#include <unordered_set>
#include "commit/Commit.h"
#include "filemanager/RepositoryConfigManager.h"
#include "filemanager/CommitsManager.h"
#include "filemanager/IgnoreFileManager.h"

using FileHashMap = std::unordered_map<std::string, size_t>;

class Repository {
public:
    Repository(std::string repositoryName,
               const std::string &repositoryFolder);

    Repository(std::string repositoryName,
               const std::string &repositoryFolder,
               FileHashMap files);

    Repository(std::string repositoryName,
               const std::string &repositoryFolder,
               const std::vector<Commit> &commits);

    ~Repository();

public:
    FileHashMap ChangedFiles() const;

private:
    [[nodiscard]] FileHashMap CollectFiles() const;

    std::unordered_set<File> FilterCollectedFiles(
            const FileHashMap &collectedFiles);

public:
    void DoCommit(std::string_view message);

    static std::tuple<int, int, int> CountFilesStatuses(
            const std::unordered_set<File> &files);

    static void SaveCommitFiles(const Commit &commit);

    static void RestoreCommitFiles(size_t checksum);

public:
    void InitConfigManager();

    void InitIgnoreManager();

    void InitCommitsManager();

    void InitManagers();

public:
    [[nodiscard]] const std::string &Name() const;

    [[nodiscard]] const std::string &Folder() const;

    [[nodiscard]] const std::vector<Commit> &Commits() const;

    [[nodiscard]] const FileHashMap &Map() const;

private:
    std::string repositoryName_;
    std::string repositoryFolder_;

    RepositoryConfigManager configManager_;
    CommitsManager commitsManager_;
    IgnoreFileManager ignoreFileManager_;

    std::vector<Commit> commits_;
    std::unordered_set<std::string> ignoredFiles_;

    FileHashMap fileHashMap_;
};

#endif //CODEHUB_REPOSITORY_H
