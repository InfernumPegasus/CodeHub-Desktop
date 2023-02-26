#ifndef CODEHUB_REPOSITORYCONFIGMANAGER_H
#define CODEHUB_REPOSITORYCONFIGMANAGER_H

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class RepositoryConfigManager {
public:
    using FileHashMap = std::unordered_map<std::string, size_t>;

public:
    RepositoryConfigManager(std::string configFile,
                            std::string *repositoryNameRef,
                            std::string *repositoryFolderRef,
                            FileHashMap *fileHashMapRef);

    RepositoryConfigManager(const RepositoryConfigManager &rhs) = default;

    RepositoryConfigManager &operator=(const RepositoryConfigManager &rhs) = delete;

    RepositoryConfigManager(RepositoryConfigManager &&rhs) noexcept = default;

    RepositoryConfigManager &operator=(RepositoryConfigManager &&rhs) noexcept = delete;

public:
    [[nodiscard]] bool CreateConfigFile() const;

    void UpdateConfigFile() const;

    [[nodiscard]] bool ReadConfigFile() const;

public:
    static constexpr std::string VCS_CONFIG_DIRECTORY = ".config";

private:
    std::string configFile_;

    std::string &repositoryNameRef_;
    std::string &repositoryFolderRef_;
    FileHashMap &fileHashMapRef_;
};


#endif //CODEHUB_REPOSITORYCONFIGMANAGER_H
