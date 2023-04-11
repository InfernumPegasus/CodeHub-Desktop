#include <iostream>
#include "WebService.h"
#include "../serializer/JsonSerializer.h"
#include "../filemanager/Configs.h"

cpr::Cookies WebService::GetCookiesFromFile() {
    std::cout << "in GetCookiesFromFile\n";
    std::ifstream ifs(CONFIG_COOKIES_FILE);
    cpr::Cookies cookies;
    if (ifs && !std::filesystem::is_empty(CONFIG_COOKIES_FILE)) {
        nlohmann::json cookiesJson = nlohmann::json::parse(ifs);
        cookies = JsonSerializer::CookiesFromJson(cookiesJson);
    }
    std::cout << "end GetCookiesFromFile\n";
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

size_t WebService::GetCurrentUser() {
    auto response = cpr::Get(
            cpr::Url{std::string{BASE_USERS_URL} + "me"},
            GetCookiesFromFile()
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    return json["id"];
}

/* Commits */

cpr::Response WebService::PostCommit(const Commit &commit) {
    std::cout << "in PostCommit\n";
    auto ids = WebService::PostFiles(commit.Files());

    nlohmann::json payload;
    payload["files"] = ids;
    payload["message"] = commit.Message();

    std::cout << "end PostCommit\n";
    return cpr::Post(
            cpr::Url{BASE_COMMITS_URL},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{payload.dump()}
    );
}

std::vector<size_t> WebService::PostCommits(const std::vector<Commit> &commits) {
    std::cout << "in PostCommits\n";
    std::vector<size_t> ids;
    for (const auto &commit: commits) {
        auto response = PostCommit(commit);
        nlohmann::json json = nlohmann::json::parse(response.text);
        int id = json["id"];
        ids.push_back(id);
    }
    std::cout << "end PostCommits\n";
    return ids;
}

Commit WebService::GetCommit(size_t id) {
    auto response = cpr::Get(
            cpr::Url{BASE_COMMITS_URL},
            GetCookiesFromFile(),
            cpr::Body({"id", std::to_string(id)})
    );
    nlohmann::json json = nlohmann::json::parse(response.text);
    return JsonSerializer::CommitFromJson(json);
}

std::vector<Commit> WebService::GetCommits(const std::vector<size_t> &ids) {
    std::vector<Commit> commits;
    for (const auto &id: ids) {
        commits.push_back(GetCommit(id));
    }
    return commits;
}

/* Files */

std::vector<size_t> WebService::PostFiles(const std::unordered_set<File> &files) {
    std::cout << "in PostFiles\n";
    std::vector<size_t> ids;
    for (const auto &file: files) {
        auto response = PostFile(file);
        std::cout << 1 << "\n";
        nlohmann::json json = nlohmann::json::parse(response.text);
        std::cout << 2 << "\n";
        int id = json["id"];
        ids.push_back(id);
    }
    std::cout << "end PostFiles\n";
    return ids;
}

cpr::Response WebService::PostFile(const File &file) {
    auto payload = JsonSerializer::FileToJson(file);
    std::cout << payload.dump(2) << "\n";
    auto response = cpr::Post(
            cpr::Url{BASE_FILES_URL},
//            cpr::Header{{"Content-Type", "multipart/form-data"}},
            GetCookiesFromFile(),
//            cpr::Body{
//                payload.dump()
//            },
//            cpr::File{file.Name()}
            cpr::Multipart{
                    {"file_name",   file.Name()},
                    {"file_status", static_cast<int>(file.Status())},
                    {"file_hash",   file.Hash()},
                    {"file",        cpr::File(file.Name())}
            }
    );
    std::cout << "end PostFile\n";
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
        std::cout << repoName << "\n";
        for (const auto &commit: commits) {
            std::cout << commit << "\n";
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
    return JsonSerializer::RepositoryFromJson(json);
}

/*
 * TODO все плохо((
 */
cpr::Response WebService::PostRepository(
        const Repository &repository,
        bool isPrivate) {
    auto userId = GetCurrentUser();
    std::cout << "user: " << userId << "\n";
    auto payload = JsonSerializer::RepositoryToJson(repository);
//    nlohmann::json payload;
    payload["repo_name"] = repository.Name();
    payload["owner"] = userId;
    payload["is_private"] = isPrivate;

    std::cout << payload.dump(2) << "\n";
    std::cout << "before PostCommits\n";
    auto commitsIds = WebService::PostCommits(repository.Commits());

    // TODO
    payload["commits"] = 2;

    std::cout << payload.dump(2) << "\n";

    auto filesSet = repository.MapToFilenames();
//    auto files = AttachFiles(filesSet);

    return cpr::Post(
            cpr::Url{BASE_REPOSITORIES_URL}
//            cpr::Header{{"Content-Type", "multipart/form-data"}},
//            cpr::Body{
//                    payload.dump()
//            },
//            cpr::Multipart{
//                    {"json_data", payload.dump(), "application/json"},
//                    {"file_data", files}
//            }
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

cpr::Files WebService::AttachFiles(const std::unordered_set<std::string> &filesVector) {
    cpr::Files files;
    for (const auto &file: filesVector) {
        cpr::File f(file);
        files.emplace_back(f);
    }
    return files;
}

