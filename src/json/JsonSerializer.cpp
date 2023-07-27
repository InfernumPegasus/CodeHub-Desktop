#include "json/JsonSerializer.h"

#include <fmt/format.h>

#include <iostream>

#include "utils/ConfigFiles.h"

nlohmann::json JsonSerializer::FileToJson(const File& file) {
  nlohmann::json j;
  j["file_name"] = file.Name();
  j["file_hash"] = file.Hash();
  j["file_status"] = file.Status();
  return j;
}

File JsonSerializer::FileFromJson(nlohmann::json json) {
  const std::string name = json["file_name"];
  const std::size_t hash = json["file_hash"];
  const FileStatus status = json["file_status"];
  return {name, hash, status};
}

File JsonSerializer::FileFromWebJson(nlohmann::json json) {
  const std::string name = json["file_name"];
  const std::string hash = json["file_hash"];
  const FileStatus status = json["file_status"];
  return {name, std::stoull(hash), status};
}

nlohmann::json JsonSerializer::CommitToJson(const Commit& commit) {
  nlohmann::json j;
  std::list<nlohmann::json> files;
  for (const auto& file : commit.Files()) {
    files.push_back(FileToJson(file));
  }
  j["files"] = files;
  j["message"] = commit.Message();
  j["commit_hash"] = commit.Checksum();
  return j;
}

Commit JsonSerializer::CommitFromJson(nlohmann::json json) {
  types::FilesSet files;
  for (const auto& file : json["files"]) {
    files.insert(FileFromJson(file));
  }
  const std::string message = json["message"];
  const std::size_t checkSum = json["commit_hash"];
  return {files, message, checkSum};
}

Commit JsonSerializer::CommitFromWebJson(nlohmann::json json) {
  types::FilesSet files;
  for (const auto& file : json["files"]) {
    files.insert(FileFromWebJson(file));
  }
  const std::string message = json["message"];
  const std::string checkSum = json["commit_hash"];
  return {files, message, stoull(checkSum)};
}

nlohmann::json JsonSerializer::RepositoryToConfigJson(std::string_view repoName,
                                                      std::string_view repoFolder,
                                                      const types::FileHashMap& files,
                                                      const types::Branch& branch) {
  nlohmann::json j;
  j["repo_name"] = repoName;
  j["repo_folder"] = repoFolder;
  j["map"] = files;
  j["current_branch"] = branch;
  return j;
}

nlohmann::json JsonSerializer::CommitsToJson(const types::Commits& commits) {
  nlohmann::json j;
  std::list<nlohmann::json> commitsJson;
  for (const auto& commit : commits) {
    commitsJson.push_back(CommitToJson(commit));
  }
  j["commits"] = commitsJson;
  return j;
}

//Repository JsonSerializer::RepositoryFromConfigJson(nlohmann::json json) {
//  const std::string name = json["repo_name"];
//  const std::string folder = json["repo_folder"];
//  const auto map = json["map"];
//  const std::string branch = json["current_branch"];
//  return {name, folder, map, branch};
//}

types::Commits JsonSerializer::CommitsFromJson(nlohmann::json json) {
  if (json.empty()) return {};
  const std::list<nlohmann::json> commitsJson = json["commits"];
  types::Commits commits;
  for (const auto& commit : commitsJson) {
    commits.push_back(CommitFromJson(commit));
  }
  return commits;
}

//std::optional<types::Commits> JsonSerializer::CommitsFromWebJson(nlohmann::json json) {
//  if (json.empty()) return {};
//  const std::list<nlohmann::json> commitsJson = json["commits"];
//  types::Commits commits;
//  for (const auto& commit : commitsJson) {
//    commits.push_back(CommitFromWebJson(commit));
//  }
//  return commits;
//}

//Repository JsonSerializer::GetRepositoryNameByFolder(const std::filesystem::path& folder) {
//  const auto file{folder / CONFIG_DIRECTORY / REPOSITORY_CONFIG_FILE};
//  std::ifstream ifs(file);
//  if (!ifs) {
//    throw std::runtime_error(fmt::format("Cannot open file '{}'", file.c_str()));
//  }
//
//  const auto j = nlohmann::json::parse(ifs);
//  return RepositoryFromConfigJson(j);
//}

nlohmann::json JsonSerializer::RepositoryToJson(const Repository& repository) {
  nlohmann::json json;
  json["repo_name"] = repository.Name();
  json["commits"] = CommitsToJson(repository.Commits())["commits"];
  return json;
}

//Repository JsonSerializer::RepositoryFromWebJson(nlohmann::json json) {
//  const std::string name = json["repo_name"];
//  const auto folder = std::filesystem::current_path();
//  const auto commits = CommitsFromWebJson(json);
//  return {name, folder, commits.value()};
//}

nlohmann::json JsonSerializer::BranchesToJson(const types::Branches& branches) {
  nlohmann::json json;
  json["branches"] = branches;
  return json;
}

types::Branches JsonSerializer::BranchesFromJson(nlohmann::json json) {
  return json["branches"];
}

nlohmann::json JsonSerializer::CookiesToJson(const cpr::Cookies& cookies) {
  nlohmann::json json;
  for (const auto& cookie : cookies) {
    json[cookie.GetName()] = cookie.GetValue();
  }
  return json;
}

cpr::Cookies JsonSerializer::CookiesFromJson(nlohmann::json json) {
  if (json.empty()) {
    std::cout << "Cookies file does not exist.\n";
    throw std::runtime_error("Cookies file does not exist.");
  }
  cpr::Cookies cookies;
  cookies.emplace_back({"access-token", json["access-token"]});
  cookies.emplace_back({"refresh-token", json["refresh-token"]});
  return cookies;
}

cpr::Cookies JsonSerializer::GetCookiesFromFile() {
  std::ifstream ifs(CONFIG_COOKIES_FILE);
  if (!ifs || !std::filesystem::exists(CONFIG_COOKIES_FILE) ||
      std::filesystem::is_empty(CONFIG_COOKIES_FILE))
    return {};

  cpr::Cookies cookies;
  if (ifs && !std::filesystem::is_empty(CONFIG_COOKIES_FILE)) {
    nlohmann::json cookiesJson = nlohmann::json::parse(ifs);
    cookies = JsonSerializer::CookiesFromJson(cookiesJson);
  }
  return cookies;
}

void JsonSerializer::SaveCookiesInFile(const cpr::Cookies& cookies) {
  std::ofstream ofs(CONFIG_COOKIES_FILE);
  const auto json = JsonSerializer::CookiesToJson(cookies);
  ofs << json.dump(2);
}
