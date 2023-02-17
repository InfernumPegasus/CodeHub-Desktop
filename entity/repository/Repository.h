#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H


#include <iostream>
#include <fstream>
#include "../commit/Commit.h"

class Repository {
public:
    using FileHashMap = std::unordered_map<std::string, size_t>;

public:
    // TODO сделать валидацию имени папки типа "../../folder/"
    // TODO сделать валидацию имени репозитория
    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder) :
            repositoryName_(repositoryName),
            repositoryFolder_(std::filesystem::absolute(repositoryFolder)),
            configFile_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_CONFIG_FILE),
            ignoreFile_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_IGNORE_FILE),
            commitsFile_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_COMMITS_FILE) {}

public:
    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder,
               FileHashMap files) :
            repositoryName_(repositoryName),
            repositoryFolder_(repositoryFolder),
            fileHashMap_(std::move(files)) {}

private:
    // TODO вынести в отдельный класс работу с конфигами
    // TODO сделать так чтобы папка в игнор листе автоматически игнорила все ее содержимое
    bool CreateIgnoreFile();

    bool ReadIgnoreFile();

private:
    [[nodiscard]] bool CreateConfigFile() const;

    void UpdateConfigFile();

    bool ReadConfigFile();

private:
    bool CreateCommitsFile() const;

    void UpdateCommitsFile() const;

    bool ReadCommitsFile();

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

    std::string configFile_;
    std::string ignoreFile_;
    std::string commitsFile_;

    std::vector<Commit> commits_;
    std::set<std::string> ignoredFiles_;

    FileHashMap fileHashMap_;
};


#endif //CODEHUB_REPOSITORY_H
