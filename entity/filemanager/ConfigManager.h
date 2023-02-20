#ifndef CODEHUB_CONFIGMANAGER_H
#define CODEHUB_CONFIGMANAGER_H

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    using FileHashMap = std::unordered_map<std::string, size_t>;

public:
    ConfigManager(std::string configFile,
                  std::string *repositoryNameRef,
                  std::string *repositoryFolderRef,
                  FileHashMap *fileHashMapRef);

    ConfigManager(const ConfigManager &rhs) = default;

    ConfigManager &operator=(const ConfigManager &rhs) = delete;

    ConfigManager(ConfigManager &&rhs) noexcept = default;

    ConfigManager &operator=(ConfigManager &&rhs) noexcept = delete;

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


#endif //CODEHUB_CONFIGMANAGER_H
