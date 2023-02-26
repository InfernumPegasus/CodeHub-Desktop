#ifndef CODEHUB_JSONSERIALIZER_H
#define CODEHUB_JSONSERIALIZER_H

#include <nlohmann/json.hpp>
#include "../entity/repository/Repository.h"

class JsonSerializer {
public:
    using NameFolderMap = std::unordered_map<std::string, std::string>;

public:
    static nlohmann::json FileToJson(const File &file);

    static File FileFromJson(nlohmann::json json);

public:
    static nlohmann::json CommitToJson(const Commit &commit);

    static Commit CommitFromJson(nlohmann::json json);

public:
    static nlohmann::json ConfigToJson(
            std::string_view name,
            std::string_view folder,
            Repository::FileHashMap &fileHashMap);

    static nlohmann::json CommitsToJson(const std::vector<Commit> &commits);

    static Repository ConfigFromJson(nlohmann::json json);

    static std::vector<Commit> CommitsFromJson(nlohmann::json json);

public:
    static nlohmann::json AppConfigToJson(const NameFolderMap &map);

    static NameFolderMap AppConfigFromJson(nlohmann::json json);
};


#endif //CODEHUB_JSONSERIALIZER_H
