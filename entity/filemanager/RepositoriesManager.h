#ifndef CODEHUB_REPOSITORIESMANAGER_H
#define CODEHUB_REPOSITORIESMANAGER_H

#include <unordered_map>
#include <string>

class RepositoriesManager {
public:
    using NameFolderMap = std::unordered_map<std::string, std::string>;

public:
    RepositoriesManager() :
            appConfigDirectory_(GetHomeDirectory()),
            repositoriesFile_(appConfigDirectory_ + "/" + REPOSITORIES_FILE) {}

public:
    static std::string GetHomeDirectory();

public:
    [[nodiscard]] bool CreateConfigFile() const;

    void UpdateConfigFile() const;

    [[nodiscard]] bool ReadConfigFile();

public:
    void AddRepository(std::string_view repositoryName,
                       std::string_view repositoryFolder);

public:
    NameFolderMap NameAndFolderMap() const;

private:
    static constexpr std::string REPOSITORIES_FILE = "repos.json";

private:
    const std::string appConfigDirectory_;
    const std::string repositoriesFile_;

    NameFolderMap nameAndFolderMap_;
};


#endif //CODEHUB_REPOSITORIESMANAGER_H
