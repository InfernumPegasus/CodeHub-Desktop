#include <iostream>
#include "WebService.h"
#include "../serializer/JsonSerializer.h"
#include "../config/ConfigFiles.h"

cpr::Cookies WebService::GetCookiesFromFile() {
    std::ifstream ifs(CONFIG_COOKIES_FILE);
    if (!ifs ||
        !fs::exists(CONFIG_COOKIES_FILE) ||
        fs::is_empty(CONFIG_COOKIES_FILE))
        return {};

    cpr::Cookies cookies;
    if (ifs && !std::filesystem::is_empty(CONFIG_COOKIES_FILE)) {
        nlohmann::json cookiesJson = nlohmann::json::parse(ifs);
        cookies = JsonSerializer::CookiesFromJson(cookiesJson);
    }
    return cookies;
}

void WebService::SaveCookiesInFile(const cpr::Cookies &cookies) {
    std::ofstream ofs(CONFIG_COOKIES_FILE);
    auto json = JsonSerializer::CookiesToJson(cookies);
    ofs << json.dump(2);
}

/* Users */

cpr::Response WebService::PostLogin() {
    if (!fs::exists(CONFIG_USER_FILE) ||
        fs::is_empty(CONFIG_USER_FILE)) {
        std::cout << "No user.\n";
        return {};
    }
    std::ifstream ifs(CONFIG_USER_FILE);
    nlohmann::json j = nlohmann::json::parse(ifs);
    auto response = cpr::Post(
            cpr::Url{BASE_TOKENS_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{j.dump()}
    );

    nlohmann::json cookiesJson = nlohmann::json::parse(response.text);
    SaveCookiesInFile(JsonSerializer::CookiesFromJson(cookiesJson));
    return response;
}

int WebService::GetCurrentUser() {
    auto response = cpr::Get(
            cpr::Url{BASE_USERS_URL + "me"},
            GetCookiesFromFile()
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    return json["id"];
}

/* Commits */

cpr::Response WebService::PostCommit(
        const Commit &commit) {
    auto ids = WebService::PostFiles(commit.Files());

    nlohmann::json payload;
    payload["files"] = ids;
    payload["message"] = commit.Message();
    payload["commit_hash"] = commit.Checksum();

    return cpr::Post(
            cpr::Url{BASE_COMMITS_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{payload.dump()}
    );
}

std::vector<int> WebService::PostCommits(
        const std::vector<Commit> &commits) {
    std::vector<int> ids;
    for (const auto &commit: commits) {
        auto response = PostCommit(commit);
        nlohmann::json json = nlohmann::json::parse(response.text);
        int id = json["id"];
        ids.push_back(id);
    }
    return ids;
}

/* Files */

std::vector<int> WebService::PostFiles(
        const std::unordered_set<File> &files) {
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
    auto payload = JsonSerializer::FileToJson(file);
    auto response = cpr::Post(
            cpr::Url{BASE_FILES_URL},
            GetCookiesFromFile(),
            cpr::Multipart{
                    {"file_name",   file.Name()},
                    {"file_status", static_cast<int>(file.Status())},
                    {"file_hash",   std::to_string(file.Hash())},
                    {"file",        cpr::File(file.Name())}
            }
    );
    return response;
}

/* Repositories */

std::optional<Repository> WebService::GetRepository(
        const std::string &repoName) {
    try {
        return JsonSerializer::RepositoryFromWebJson(
                GetRepositoryJson(repoName)
        );
    } catch (nlohmann::json::exception &e) {
        std::cout << "Cannot get repo!\n";
        return {};
    }
}

nlohmann::json WebService::GetRepositoryJson(const std::string &repoName) {
    auto cookies = GetCookiesFromFile();
    auto response = cpr::Get(
            cpr::Url{std::string{BASE_REPOSITORIES_URL}},
            cookies,
            cpr::Payload{
                    {"repo_name", repoName}
            }
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    return json;
}

cpr::Response WebService::PostRepository(
        const Repository &repository) {
    auto payload = JsonSerializer::RepositoryToJson(repository);
    payload["repo_name"] = repository.Name();
    payload["is_private"] = true;
    payload["commits"] = WebService::PostCommits(repository.Commits());
    std::cout << payload.dump(2) << "\n";

    return cpr::Post(
            cpr::Url{BASE_REPOSITORIES_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{payload.dump()},
            GetCookiesFromFile()
    );
}

/*
 * TODO перделать (зачем?)
 */
cpr::Response WebService::PatchRepository(
        std::string_view repoName,
        const Repository &repository,
        bool isPrivate,
        const std::vector<Commit> &commits) {
    const auto commitsIds = PostCommits(commits);
    const auto repoJson = GetRepositoryJson(repoName.data());
    const int repoId = repoJson["id"];
//    auto currentRepo =
//            JsonSerializer::RepositoryFromWebJson(repoJson);
    const int userId = GetCurrentUser();
    auto payload = JsonSerializer::RepositoryToJson(repository);
    payload["owner"] = userId;
    payload["is_private"] = isPrivate;
    payload["commits"] = commitsIds;

    return cpr::Patch(
            cpr::Url{BASE_REPOSITORIES_URL + std::to_string(repoId) + '/'},
            GetCookiesFromFile(),
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{payload.dump()}
    );
}

