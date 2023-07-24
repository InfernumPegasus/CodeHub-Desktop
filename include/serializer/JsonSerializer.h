#ifndef CODEHUB_JSONSERIALIZER_H
#define CODEHUB_JSONSERIALIZER_H

#include <cpr/cookies.h>

#include <nlohmann/json.hpp>
#include <optional>

#include "commit/Commit.h"
#include "utils/Types.h"
#include "repository/Repository.h"

class JsonSerializer {
 public:
  static nlohmann::json FileToJson(const File& file);

  static File FileFromJson(nlohmann::json json);

  static File FileFromWebJson(nlohmann::json json);

 public:
  static nlohmann::json CommitToJson(const Commit& commit);

  static Commit CommitFromJson(nlohmann::json json);

  static Commit CommitFromWebJson(nlohmann::json json);

 public:
  static nlohmann::json RepositoryToConfigJson(std::string_view repoName,
                                               std::string_view repoFolder,
                                               const types::FileHashMap& files,
                                               const std::string& branch);

  static nlohmann::json CommitsToJson(const types::Commits& commits);

  static Repository RepositoryFromConfigJson(nlohmann::json json);

  static std::optional<types::Commits> CommitsFromJson(nlohmann::json json);

  static std::optional<types::Commits> CommitsFromWebJson(nlohmann::json json);

 public:
  static Repository GetRepositoryByFolder(const std::filesystem::path& folder);

 public:
  static nlohmann::json RepositoryToJson(const Repository& repository);

  static Repository RepositoryFromWebJson(nlohmann::json json);

 public:
  static nlohmann::json BranchesToJson(const types::Branches& branches);

  static types::Branches BranchesFromJson(nlohmann::json json);

 public:
  static nlohmann::json CookiesToJson(const cpr::Cookies& cookies);

  static cpr::Cookies CookiesFromJson(nlohmann::json json);

  static cpr::Cookies GetCookiesFromFile();

  static void SaveCookiesInFile(const cpr::Cookies& cookies);
};

#endif  // CODEHUB_JSONSERIALIZER_H
