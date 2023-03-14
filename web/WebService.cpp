#include <iostream>
#include "WebService.h"
#include "../serializer/JsonSerializer.h"

cpr::Response WebService::PostLogin(std::string_view pathToJson) {
    std::ifstream ifs(pathToJson.data());
    nlohmann::json j = nlohmann::json::parse(ifs);
    return PostLogin(j);
}

cpr::Response WebService::PostLogin(const nlohmann::json &userJson) {
    return cpr::Post(
            cpr::Url{BASE_TOKENS_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{userJson.dump()}
    );
}

std::vector<int> WebService::PostFiles(const std::set<File> &files) {
    std::vector<int> ids;
    for (const auto &file: files) {
        auto response = PostFile(file);
        nlohmann::json json = nlohmann::json::parse(response.text);
        int id = json["id"];
        ids.push_back(id);
    }
    return ids;
}

cpr::Response WebService::PostFile(const File &file) {
    return cpr::Post(
            cpr::Url{BASE_FILES_URL},
            cpr::Multipart{
                    {"file_status", static_cast<int>(file.Status())},
                    {"file_hash",   std::to_string(file.Hash())},
                    {"file",        cpr::File(file.Name())}
            }
    );
}

cpr::Response WebService::PostCommit(const Commit &commit) {
    auto ids = WebService::PostFiles(commit.Files());

    nlohmann::json payload;
    payload["files"] = ids;
    payload["message"] = commit.Message();

    return cpr::Post(
            cpr::Url{BASE_COMMITS_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{payload.dump()}
    );
}

std::vector<int> WebService::PostCommits(const std::vector<Commit> &commits) {
    std::vector<int> ids;
    for (const auto &commit: commits) {
        auto response = PostCommit(commit);
        nlohmann::json json = nlohmann::json::parse(response.text);
        int id = json["id"];
        ids.push_back(id);
    }
    return ids;
}

void WebService::GetUser(int id) {
    cpr::Response response = cpr::Get(
            cpr::Url{BASE_USERS_URL.data() + std::to_string(id)}
    );
    std::cout << response.text << "\n";
}

Commit WebService::GetCommit(size_t hash) {
    auto response = cpr::Get(
            cpr::Url{BASE_COMMITS_URL},
            cpr::Body({"hash", std::to_string(hash)})
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    std::vector<int> ids = json["files"];
    std::string message = json["message"];

    auto files = GetFiles(ids);
    return {files, message};
}

std::vector<Commit> WebService::GetCommits(const std::vector<int> &ids) {
    std::vector<Commit> commits;
    for (const auto &id: ids) {
        commits.push_back(GetCommit(id));
    }
    return commits;
}

std::set<File> WebService::GetFiles(const std::vector<int> &ids) {
    std::set<File> files;
    for (const auto &id: ids) {
        files.insert(GetFile(id));
    }
    return files;
}

// TODO сделать получение файла
File WebService::GetFile(int id) {
    auto response = cpr::Get(
            cpr::Url{std::string{BASE_FILES_URL} + std::to_string(id)}
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
}

