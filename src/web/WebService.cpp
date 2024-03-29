#include "web/WebService.h"

#include <iostream>

#include "json/JsonSerializer.h"

bool WebService::NoErrorsInResponse(long statusCode) {
  using namespace cpr::status;
  const auto code = static_cast<int32_t>(statusCode);
  return !is_client_error(code) && !is_server_error(code);
}

/* Users */

cpr::Response WebService::PostLogin(std::string_view email, std::string_view password) {
  nlohmann::json j;
  j["email"] = email;
  j["password"] = password;
  auto response =
      cpr::Post(cpr::Url{BASE_TOKENS_URL},
                cpr::Header{{"Content-Type", "application/json"}}, cpr::Body{j.dump()});

  nlohmann::json cookiesJson = nlohmann::json::parse(response.text);
  JsonSerializer::SaveCookiesInFile(JsonSerializer::CookiesFromJson(cookiesJson));
  return response;
}

int WebService::GetCurrentUser() {
  const auto response =
      cpr::Get(cpr::Url{BASE_USERS_URL + "me"}, JsonSerializer::GetCookiesFromFile());
  const nlohmann::json json = nlohmann::json::parse(response.text);
  return json["id"];
}

/* Commits */

cpr::Response WebService::PostCommit(const Commit& commit) {
  const auto ids = WebService::PostFiles(commit.Files());

  nlohmann::json payload;
  payload["files"] = ids;
  payload["message"] = commit.Message();
  payload["commit_hash"] = commit.Checksum();

  return cpr::Post(cpr::Url{BASE_COMMITS_URL},
                   cpr::Header{{"Content-Type", "application/json"}},
                   cpr::Body{payload.dump()});
}

std::vector<int> WebService::PostCommits(const types::Commits& commits) {
  std::vector<int> ids;
  for (const auto& commit : commits) {
    const auto response = PostCommit(commit);
    const nlohmann::json json = nlohmann::json::parse(response.text);
    const int id = json["id"];
    ids.push_back(id);
  }
  return ids;
}

/* FilesSet */

std::vector<int> WebService::PostFiles(const types::FilesSet& files) {
  std::vector<int> ids;
  for (const auto& file : files) {
    const auto response = PostFile(file);
    try {
      const nlohmann::json json = nlohmann::json::parse(response.text);
      const int id = json["id"];
      ids.push_back(id);
    } catch (nlohmann::json::exception& e) {
    }
  }
  return ids;
}

cpr::Response WebService::PostFile(const File& file) {
  return cpr::Post(cpr::Url{BASE_FILES_URL}, JsonSerializer::GetCookiesFromFile(),
                   cpr::Multipart{{"file_name", file.Name()},
                                  {"file_status", static_cast<int>(file.Status())},
                                  {"file_hash", std::to_string(file.Hash())},
                                  {"file", cpr::File(file.Name())}});
}

/* Repositories */

std::optional<Repository> WebService::GetRepository(const std::string& repoName) {
  try {
    // TODO
//    return JsonSerializer::RepositoryFromWebJson(GetRepositoryJson(repoName)));
    return {};
  } catch (nlohmann::json::exception& e) {
    std::cout << "Cannot get repo!\n";
    return {};
  }
}

nlohmann::json WebService::GetRepositoryJson(const std::string& repoName) {
  const auto response = cpr::Get(cpr::Url{std::string{BASE_REPOSITORIES_URL}},
                                 JsonSerializer::GetCookiesFromFile(),
                                 cpr::Payload{{"repo_name", repoName}});
  nlohmann::json json = nlohmann::json::parse(response.text);
  return json;
}

cpr::Response WebService::PostRepository(const Repository& repository) {
  auto payload = JsonSerializer::RepositoryToJson(repository);
  payload["repo_name"] = repository.Name();
  payload["is_private"] = true;
  payload["commits"] = WebService::PostCommits(repository.Commits());

  return cpr::Post(cpr::Url{BASE_REPOSITORIES_URL},
                   cpr::Header{{"Content-Type", "application/json"}},
                   cpr::Body{payload.dump()}, JsonSerializer::GetCookiesFromFile());
}

cpr::Response WebService::PatchRepository(std::string_view repoName,
                                          const Repository& repository, bool isPrivate,
                                          const types::Commits& commits) {
  const auto commitsIds = PostCommits(commits);
  const auto repoJson = GetRepositoryJson(repoName.data());
  const int repoId = repoJson["id"];
  const int userId = GetCurrentUser();
  auto payload = JsonSerializer::RepositoryToJson(repository);
  payload["owner"] = userId;
  payload["is_private"] = isPrivate;
  payload["commits"] = commitsIds;

  auto url = BASE_REPOSITORIES_URL;
  return cpr::Patch(cpr::Url{url.append(std::to_string(repoId)).append("/")},
                    JsonSerializer::GetCookiesFromFile(),
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Body{payload.dump()});
}
