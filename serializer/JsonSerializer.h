#ifndef CODEHUB_JSONSERIALIZER_H
#define CODEHUB_JSONSERIALIZER_H

#include "../entity/repository/Repository.h"
#include <nlohmann/json.hpp>

template<class T>
class JsonSerializer;

template<>
class JsonSerializer<File> {
public:
    static nlohmann::json ToJson(const File &file) {
        nlohmann::json j;
        j["name"] = file.Name();
        j["hash"] = file.Hash();
        j["status"] = file.Status();
        return j;
    }

    static File FromJson(nlohmann::json json) {
        std::string name = json["name"];
        size_t hash = json["hash"];
        FileStatus status = json["status"];
        return {name, hash, status};
    }
};

template<>
class JsonSerializer<Commit> {
public:
    static nlohmann::json ToJson(const Commit &commit) {
        nlohmann::json j;
        std::vector<nlohmann::json> files;
        for (const auto &file: commit.Files()) {
            files.push_back(JsonSerializer<File>::ToJson(file));
        }
        j["files"] = files;
        j["message"] = commit.Message();
        j["checksum"] = commit.Checksum();
        return j;
    }

    static Commit FromJson(nlohmann::json json) {
        std::set<File> files;
        for (auto &file: json["files"]) {
            files.insert(JsonSerializer<File>::FromJson(file));
        }
        std::string message = json["message"];
        size_t checkSum = json["checksum"];
        return {files, message, checkSum};
    }
};

template<>
class JsonSerializer<Repository> {
public:
    static nlohmann::json ConfigToJson(std::string_view name,
                                       std::string_view folder,
                                       Repository::FileHashMap &fileHashMap) {
        nlohmann::json j;
        j["repo_name"] = name;
        j["repo_folder"] = folder;
        j["map"] = fileHashMap;

        return j;
    }

    static nlohmann::json CommitsToJson(const std::vector<Commit> &commits) {
        nlohmann::json j;
        std::vector<nlohmann::json> commitsJson;
        for (const auto &commit: commits) {
            commitsJson.push_back(JsonSerializer<Commit>::ToJson(commit));
        }
        j["commits"] = commitsJson;
        return j;
    }

    static Repository ConfigFromJson(nlohmann::json json) {
        std::string repositoryName = json["repo_name"];
        std::string repositoryFolder = json["repo_folder"];
        auto fileHashMap = json["map"];

        return {repositoryName, repositoryFolder, fileHashMap};
    }

    static std::vector<Commit> CommitsFromJson(nlohmann::json json) {
        std::cout << json.dump(2) << std::endl;
        std::vector<nlohmann::json> commitsJson = json["commits"];
        for (const auto &item: commitsJson) {
            std::cout << item.dump(2) << "\n";
        }
        std::vector<Commit> commits;
        for (const auto &commit: commitsJson) {
            commits.push_back(JsonSerializer<Commit>::FromJson(commit));
        }
        return commits;
    }
};


#endif //CODEHUB_JSONSERIALIZER_H
