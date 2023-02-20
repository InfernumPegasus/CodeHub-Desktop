#include "ConfigManager.h"
#include "../../serializer/JsonSerializer.h"

ConfigManager::ConfigManager(
        std::string configFile,
        std::string *repositoryNameRef,
        std::string *repositoryFolderRef,
        FileHashMap *fileHashMapRef) :
        configFile_(std::move(configFile)),
        repositoryNameRef_(*repositoryNameRef),
        repositoryFolderRef_(*repositoryFolderRef),
        fileHashMapRef_(*fileHashMapRef) {}

bool ConfigManager::CreateConfigFile() const {
    std::string configDirectory = repositoryFolderRef_ + "/" + VCS_CONFIG_DIRECTORY;
    std::ofstream file;
    if ((std::filesystem::exists(configDirectory) &&
         !std::filesystem::exists(configFile_)) ||
        std::filesystem::create_directory(configDirectory)) {}
    file.open(configFile_);
    return file.is_open();
}

void ConfigManager::UpdateConfigFile() const {
    std::ofstream ofs(configFile_);
    if (!ofs && !CreateConfigFile()) {
        std::cout << "Cannot create config folder!\n";
        return;
    }

    auto repoJson = JsonSerializer::ConfigToJson(
            repositoryNameRef_,
            repositoryFolderRef_,
            fileHashMapRef_
    ).dump(2);
    ofs << repoJson;
}

bool ConfigManager::ReadConfigFile() const {
    if (!std::filesystem::exists(configFile_)) {
        return false;
    }

    if (std::filesystem::is_empty(configFile_)) {
        return true;
    }

    std::ifstream ifs(configFile_);
    if (ifs) {
        nlohmann::json j = nlohmann::json::parse(ifs);
        auto res = JsonSerializer::ConfigFromJson(j);
        repositoryNameRef_ = res.Name();
        repositoryFolderRef_ = res.Folder();
        fileHashMapRef_ = res.Map();

        return true;
    }

    return false;
}
