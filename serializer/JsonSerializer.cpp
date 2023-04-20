#include "JsonSerializer.h"
#include "../filemanager/Configs.h"

nlohmann::json JsonSerializer::FileToJson(
        const File &file) {
    nlohmann::json j;
    j["file_name"] = file.Name();
    j["file_hash"] = file.Hash();
    j["file_status"] = file.Status();
    return j;
}

File JsonSerializer::FileFromJson(
        nlohmann::json json) {
    std::string name = json["file_name"];
    int32_t hash = json["file_hash"];
    FileStatus status = json["file_status"];
    return {name, hash, status};
}

nlohmann::json JsonSerializer::CommitToJson(
        const Commit &commit) {
    nlohmann::json j;
    std::vector<nlohmann::json> files;
    for (const auto &file: commit.Files()) {
        files.push_back(FileToJson(file));
    }
    j["files"] = files;
    j["message"] = commit.Message();
    j["commit_hash"] = commit.Checksum();
    return j;
}

Commit JsonSerializer::CommitFromJson(
        nlohmann::json json) {
    std::unordered_set<File> files;
    for (auto &file: json["files"]) {
        files.insert(FileFromJson(file));
    }
    std::string message = json["message"];
    int32_t checkSum = json["commit_hash"];
    return {files, message, checkSum};
}

nlohmann::json JsonSerializer::RepositoryToConfigJson(
        std::string_view name,
        std::string_view folder,
        const FileHashMap &fileHashMap) {
    nlohmann::json j;
    j["repo_name"] = name;
    j["repo_folder"] = folder;
    j["map"] = fileHashMap;
    return j;
}

nlohmann::json JsonSerializer::CommitsToJson(
        const std::vector<Commit> &commits) {
    nlohmann::json j;
    std::vector<nlohmann::json> commitsJson;
    for (const auto &commit: commits) {
        commitsJson.push_back(CommitToJson(commit));
    }
    j["commits"] = commitsJson;
    return j;
}

std::optional<Repository> JsonSerializer::RepositoryFromConfigJson(
        nlohmann::json json) {
    std::string name = json["repo_name"];
    std::string folder = json["repo_folder"];
    auto map = json["map"];
    return std::make_optional<Repository>(name, folder, map);
}

std::vector<Commit> JsonSerializer::CommitsFromJson(
        nlohmann::json json) {
    std::vector<nlohmann::json> commitsJson = json["commits"];
    std::vector<Commit> commits;
    for (const auto &commit: commitsJson) {
        commits.push_back(CommitFromJson(commit));
    }
    return commits;
}

nlohmann::json JsonSerializer::NameFolderToJson(
        const JsonSerializer::NameFolderMap &map) {
    nlohmann::json j;
    j["map"] = map;
    return j;
}

JsonSerializer::NameFolderMap JsonSerializer::NameFolderFromJson(
        nlohmann::json json) {
    return json["map"];
}

// TODO юзнуть std::optional
std::optional<Repository> JsonSerializer::GetRepositoryByFolder(
        const std::string &folder) {
    std::ifstream ifs(folder + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_CONFIG_FILE);
    if (!ifs) {
        return {};
//        throw std::invalid_argument("Wrong folder provided!");
    }
    auto j = nlohmann::json::parse(ifs);
    return RepositoryFromConfigJson(j);
}

nlohmann::json JsonSerializer::RepositoryToJson(
        const Repository &repository) {
    nlohmann::json json;
    json["repo_name"] = repository.Name();
    json["commits"] = CommitsToJson(repository.Commits())["commits"];
    return json;
}

std::optional<Repository> JsonSerializer::RepositoryFromJson(
        nlohmann::json json) {
    if (json.empty()) return {};
    std::string name = json["repo_name"];
    auto folder = std::filesystem::current_path().string();
    auto commits = CommitsFromJson(json);
    return std::make_optional<Repository>(name, folder, commits);
}

nlohmann::json JsonSerializer::CookiesToJson(
        const cpr::Cookies &cookies) {
    nlohmann::json json;
    for (const auto &cookie: cookies) {
        json[cookie.GetName()] = cookie.GetValue();
    }
    return json;
}

cpr::Cookies JsonSerializer::CookiesFromJson(
        nlohmann::json json) {
    cpr::Cookies cookies;
    cookies.emplace_back({"access-token", json["access-token"]});
    cookies.emplace_back({"refresh-token", json["refresh-token"]});
    return cookies;
}

