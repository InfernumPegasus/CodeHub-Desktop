#ifndef CODEHUB_JSONSERIALIZER_H
#define CODEHUB_JSONSERIALIZER_H

#include <nlohmann/json.hpp>
#include "../repository/Repository.h"
#include "cpr/cookies.h"

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
    static nlohmann::json RepositoryToConfigJson(
            std::string_view name,
            std::string_view folder,
            const Repository::FileHashMap &fileHashMap);

    static nlohmann::json CommitsToJson(const std::vector<Commit> &commits);

    static Repository RepositoryFromConfigJson(nlohmann::json json);

    static std::vector<Commit> CommitsFromJson(nlohmann::json json);

public:
    static nlohmann::json NameFolderToJson(const NameFolderMap &map);

    static NameFolderMap NameFolderFromJson(nlohmann::json json);

public:
    static Repository GetRepositoryByFolder(const std::string &folder);

public:
    static nlohmann::json RepositoryToJson(const Repository &repository);

    static Repository RepositoryFromJson(nlohmann::json json);

public:
    static nlohmann::json CookiesToJson(const cpr::Cookies& cookies);

    static cpr::Cookies CookiesFromJson(nlohmann::json json);
};


#endif //CODEHUB_JSONSERIALIZER_H
