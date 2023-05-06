#ifndef CODEHUB_REPOSITORYCONFIGMANAGER_H
#define CODEHUB_REPOSITORYCONFIGMANAGER_H

#include <fstream>
#include <nlohmann/json.hpp>
#include "IFileManager.h"

class RepositoryConfigManager : public IFileManager {
public:
    using FileHashMap = std::unordered_map<std::string, size_t>;

public:
    RepositoryConfigManager(std::string configFile,
                            std::string *repositoryNameRef,
                            std::string *repositoryFolderRef,
                            FileHashMap *fileHashMapRef);

public:
    bool Create() override;

    bool Read() override;

    void Update();

private:
    std::string configFile_;

    std::string &repositoryNameRef_;
    std::string &repositoryFolderRef_;
    FileHashMap &fileHashMapRef_;
};

#endif //CODEHUB_REPOSITORYCONFIGMANAGER_H