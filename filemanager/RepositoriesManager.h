#ifndef CODEHUB_REPOSITORIESMANAGER_H
#define CODEHUB_REPOSITORIESMANAGER_H

#include <unordered_map>
#include <string>
#include "../repository/Repository.h"

class RepositoriesManager {
public:
    using NameFolderMap = std::unordered_map<std::string, std::string>;

public:
    explicit RepositoriesManager(NameFolderMap *nameAndFolderMap_);

public:
    static std::string GetHomeDirectory();

public:
    [[nodiscard]] bool CreateConfigFile() const;

    void UpdateConfigFile() const;

    bool ReadConfigFile();

private:
    static constexpr std::string REPOSITORIES_FILE = "repos.json";

private:
    const std::string appConfigDirectory_;
    const std::string repositoriesFile_;

    NameFolderMap &nameAndFolderMap_;
};


#endif //CODEHUB_REPOSITORIESMANAGER_H
