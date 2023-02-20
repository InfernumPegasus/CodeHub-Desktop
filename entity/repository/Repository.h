#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H

#include "../commit/Commit.h"
#include "../filemanager/ConfigManager.h"
#include "../filemanager/CommitsManager.h"
#include "../filemanager/IgnoreFileManager.h"

class Repository {
public:
    using FileHashMap = std::unordered_map<std::string, size_t>;

public:
    // TODO сделать валидацию имени папки типа "../../folder/"
    // TODO сделать валидацию имени репозитория
    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder);

    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder,
               FileHashMap files);

private:
    [[nodiscard]] FileHashMap CollectFiles() const;

public:
    void DoCommit(std::string_view message);

    void Init();

public:
    [[nodiscard]] std::string Name() const;

    [[nodiscard]] std::string Folder() const;

    [[nodiscard]] std::vector<Commit> Commits() const;

    [[nodiscard]] FileHashMap Map() const;

private:
    static constexpr std::string VCS_CONFIG_DIRECTORY = ".config";
    static constexpr std::string VCS_CONFIG_FILE = ".repo_info.json";
    static constexpr std::string VCS_COMMITS_FILE = ".commits.json";
    static constexpr std::string VCS_IGNORE_FILE = ".ignore";

private:
    std::string repositoryName_;
    std::string repositoryFolder_;

    ConfigManager configManager_;
    CommitsManager commitsManager_;
    IgnoreFileManager ignoreFileManager_;

    std::vector<Commit> commits_;
    std::set<std::string> ignoredFiles_;

    FileHashMap fileHashMap_;
};


#endif //CODEHUB_REPOSITORY_H
