#include "JsonSerializer.h"

nlohmann::json JsonSerializer::FileToJson(const File &file) {
    nlohmann::json j;
    j["name"] = file.Name();
    j["hash"] = file.Hash();
    j["status"] = file.Status();
    return j;
}

File JsonSerializer::FileFromJson(nlohmann::json json) {
    std::string name = json["name"];
    size_t hash = json["hash"];
    FileStatus status = json["status"];
    return {name, hash, status};
}

nlohmann::json JsonSerializer::CommitToJson(const Commit &commit) {
    nlohmann::json j;
    std::vector<nlohmann::json> files;
    for (const auto &file: commit.Files()) {
        files.push_back(FileToJson(file));
    }
    j["files"] = files;
    j["message"] = commit.Message();
    j["checksum"] = commit.Checksum();
    return j;
}

Commit JsonSerializer::CommitFromJson(nlohmann::json json) {
    std::set<File> files;
    for (auto &file: json["files"]) {
        files.insert(FileFromJson(file));
    }
    std::string message = json["message"];
    size_t checkSum = json["checksum"];
    return {files, message, checkSum};
}

nlohmann::json JsonSerializer::ConfigToJson(
        std::string_view name,
        std::string_view folder,
        Repository::FileHashMap &fileHashMap) {
    nlohmann::json j;
    j["repo_name"] = name;
    j["repo_folder"] = folder;
    j["map"] = fileHashMap;
    return j;
}

nlohmann::json JsonSerializer::CommitsToJson(const std::vector<Commit> &commits) {
    nlohmann::json j;
    std::vector<nlohmann::json> commitsJson;
    for (const auto &commit: commits) {
        commitsJson.push_back(CommitToJson(commit));
    }
    j["commits"] = commitsJson;
    return j;
}

Repository JsonSerializer::ConfigFromJson(nlohmann::json json) {
    std::string repositoryName = json["repo_name"];
    std::string repositoryFolder = json["repo_folder"];
    auto fileHashMap = json["map"];
    return {repositoryName, repositoryFolder, fileHashMap};
}

std::vector<Commit> JsonSerializer::CommitsFromJson(nlohmann::json json) {
    std::vector<nlohmann::json> commitsJson = json["commits"];
    std::vector<Commit> commits;
    for (const auto &commit: commitsJson) {
        commits.push_back(CommitFromJson(commit));
    }
    return commits;
}
