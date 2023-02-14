#ifndef CODEHUB_REPOSITORY_H
#define CODEHUB_REPOSITORY_H


#include <iostream>
#include "../commit/Commit.h"

class Repository {
public:
    using FileHashMap = std::unordered_map<std::string, int64_t>;

public:
    // TODO сделать валидацию имени папки типа "../../folder/"
    // TODO сделать валидацию имени репозитория
    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder) :
            repositoryName_(repositoryName),
            repositoryFolder_(std::filesystem::absolute(repositoryFolder)),
            configFile_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_CONFIG_FILE),
            ignoreFile_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_IGNORE_FILE) {}

private:
    Repository(std::string_view repositoryName,
               std::string_view repositoryFolder,
               std::vector<Commit> commits,
               FileHashMap fileHashMap) :
            repositoryName_(repositoryName),
            repositoryFolder_(repositoryFolder),
            commits_(std::move(commits)),
            fileHashMap_(std::move(fileHashMap)) {}

private:
    // TODO мб вынести в отдельный класс работу с конфигами?
    // TODO сделать так чтобы папка в игнор листе автоматически игнорила все ее содержимое
    bool CreateIgnoreFile();

    bool ReadIgnoreFile();

    void UpdateIgnoreFile();

private:
    [[nodiscard]] bool CreateConfigFile() const;

    void UpdateConfigFile() const;

    bool LoadConfigFile();

private:
    [[nodiscard]] FileHashMap CollectFiles() const;

public:
    void DoCommit(std::string_view message);

    void Init();

public:
    [[nodiscard]] constexpr std::string Name() const;

    [[nodiscard]] constexpr std::string Folder() const;

    [[nodiscard]] constexpr std::vector<Commit> Commits() const;

    [[nodiscard]] nlohmann::json ToJson() const;

    static Repository FromJson(nlohmann::json json);

private:
    static constexpr std::string VCS_CONFIG_DIRECTORY = ".config";
    static constexpr std::string VCS_CONFIG_FILE = ".repo_info.json";
    static constexpr std::string VCS_IGNORE_FILE = ".ignore";

private:
    std::string repositoryName_;
    std::string repositoryFolder_;

    std::string configFile_;
    std::string ignoreFile_;

    std::vector<Commit> commits_;
    std::set<std::string> ignoredFiles_;

    FileHashMap fileHashMap_;
};


#endif //CODEHUB_REPOSITORY_H
