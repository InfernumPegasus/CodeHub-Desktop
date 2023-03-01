#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include "../commit/Commit.h"
#include "../filemanager/RepositoryConfigManager.h"
#include "../filemanager/CommitsManager.h"
#include "../filemanager/IgnoreFileManager.h"

class Repository {
public:
    using FileHashMap = std::unordered_map<std::string, size_t>;

public:
    // TODO сделать валидацию имени папки типа "../../folder/"
    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder);

    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder,
               FileHashMap files);

    ~Repository();

public:
    size_t ChangedFilesAmount() const;

    void AddFiles(const std::vector<std::string> &files);

private:
    [[nodiscard]] FileHashMap CollectFiles() const;

public:
    void DoCommit(std::string_view message);

public:
    void InitConfigManager();

    void InitIgnoreManager();

    void InitCommitsManager();

    void InitManagers();

public:
    [[nodiscard]] std::string Name() const;

    [[nodiscard]] std::string Folder() const;

    [[nodiscard]] std::vector<Commit> Commits() const;

    [[nodiscard]] FileHashMap Map() const;

private:
    std::string repositoryName_;
    std::string repositoryFolder_;

    RepositoryConfigManager configManager_;
    CommitsManager commitsManager_;
    IgnoreFileManager ignoreFileManager_;

    std::vector<Commit> commits_;
    std::set<std::string> ignoredFiles_;

    FileHashMap fileHashMap_;
};


#endif //CODEHUB_REPOSITORY_H