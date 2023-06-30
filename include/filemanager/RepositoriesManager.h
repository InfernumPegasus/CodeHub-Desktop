#ifndef CODEHUB_REPOSITORIESMANAGER_H
#define CODEHUB_REPOSITORIESMANAGER_H

#include <unordered_map>
#include <string>
#include "repository/Repository.h"

class RepositoriesManager : public IFileManager {
public:
    using NameFolderMap = std::unordered_map<std::string, std::string>;

public:
    explicit RepositoriesManager(NameFolderMap *nameAndFolderMap_);

public:
    bool Create() override;

    bool Read() override;

    void Update();

private:
    const std::string appConfigDirectory_;
    const std::string repositoriesFile_;

    NameFolderMap &nameAndFolderMap_;
};

#endif //CODEHUB_REPOSITORIESMANAGER_H
