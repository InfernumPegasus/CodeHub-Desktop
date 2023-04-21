#include <iostream>
#include "WebService.h"
#include "../serializer/JsonSerializer.h"
#include "../config/Configs.h"

cpr::Cookies WebService::GetCookiesFromFile() {
    std::ifstream ifs(CONFIG_COOKIES_FILE);
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
            cpr::Url{std::string{BASE_USERS_URL} + "me"},
            GetCookiesFromFile()
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    return json["id"];
}

/* Commits */

cpr::Response WebService::PostCommit(const Commit &commit) {
    auto ids = WebService::PostFiles(commit.Files());

    nlohmann::json payload;
    payload["files"] = ids;
    payload["message"] = commit.Message();
    payload["commit_hash"] = commit.Checksum();

//    std::cout << payload.dump(2) << "\n";

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

Commit WebService::GetCommit(int id) {
    auto response = cpr::Get(
            cpr::Url{BASE_COMMITS_URL},
            GetCookiesFromFile(),
            cpr::Body({"id", std::to_string(id)})
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    return JsonSerializer::CommitFromJson(json);
}

std::vector<Commit> WebService::GetCommits(const std::vector<int> &ids) {
    std::vector<Commit> commits;
    for (const auto &id: ids) {
        commits.push_back(GetCommit(id));
    }
    return commits;
}

/* Files */

std::vector<int> WebService::PostFiles(const std::unordered_set<File> &files) {
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
//            cpr::Header{{"Content-Type", "multipart/form-data"}},
            GetCookiesFromFile(),
//            cpr::Body{
//                payload.dump()
//            },
            cpr::Multipart{
                    {"file_name",   file.Name()},
                    {"file_status", static_cast<int>(file.Status())},
                    {"file_hash",   file.Hash()},
                    {"file",        cpr::File(file.Name())}
            }
    );
    return response;
}

/* Refresh */

cpr::Response WebService::GetRefresh() {
    return cpr::Get(
            cpr::Url{BASE_REFRESH_URL},
            GetCookiesFromFile()
    );
}

/* Repositories */

/*
 * TODO проверить
 */
void WebService::GetRepositories() {
    auto response = cpr::Get(
            cpr::Url{BASE_REPOSITORIES_URL},
            GetCookiesFromFile()
    );
    nlohmann::json repositories = nlohmann::json::parse(response.text);
    for (const auto &repo: repositories) {
        std::string repoName = repo["repo_name"];
        auto commits = GetCommits(repo["commits"]);
//        std::cout << repoName << "\n";
        for (const auto &commit: commits) {
//            std::cout << commit << "\n";
        }
    }
}

Repository WebService::GetRepository(const std::string &repoName) {
    auto cookies = GetCookiesFromFile();
    auto response = cpr::Get(
            cpr::Url{std::string{BASE_REPOSITORIES_URL}},
            cookies,
            cpr::Payload{
                    {"repo_name", repoName}
            }
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    auto repository = JsonSerializer::RepositoryFromJson(json);
    if (!repository) {
        throw std::invalid_argument("Cannot get repository");
    }
    return repository.value();
}

/*
 * TODO все плохо((
 */
cpr::Response WebService::PostRepository(
        const Repository &repository,
        bool isPrivate) {
    auto userId = GetCurrentUser();
    auto payload = JsonSerializer::RepositoryToJson(repository);
    payload["repo_name"] = repository.Name();
    payload["owner"] = userId;
    payload["is_private"] = isPrivate;
    payload["commits"] = WebService::PostCommits(repository.Commits());

    return cpr::Post(
            cpr::Url{BASE_REPOSITORIES_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{
                    payload.dump()
            }
    );
}

/*
 * TODO перделать (зачем?)
 */
cpr::Response WebService::PatchRepository(
        std::string_view repoName,
        const Repository &repository,
        bool isPrivate) {
    auto currentRepo = GetRepository(repoName.data());
    auto userId = GetCurrentUser();
    auto payload = JsonSerializer::RepositoryToJson(repository);
    payload["owner"] = userId;
    payload["is_private"] = isPrivate;

    return cpr::Patch(
            cpr::Url{BASE_REPOSITORIES_URL},
            GetCookiesFromFile(),
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{payload.dump()}
    );
}

