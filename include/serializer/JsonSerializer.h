#ifndef CODEHUB_JSONSERIALIZER_H
#define CODEHUB_JSONSERIALIZER_H

#include <cpr/cookies.h>

#include <nlohmann/json.hpp>
#include <optional>

#include "repository/Repository.h"

class JsonSerializer {
 public:
  using NameFolderMap = std::unordered_map<std::string, std::string>;

 public:
  static nlohmann::json FileToJson(const File& file);

  static File FileFromJson(nlohmann::json json);

  static File FileFromWebJson(nlohmann::json json);

 public:
  static nlohmann::json CommitToJson(const Commit& commit);

  static Commit CommitFromJson(nlohmann::json json);

  static Commit CommitFromWebJson(nlohmann::json json);

 public:
  static nlohmann::json RepositoryToConfigJson(std::string_view name,
                                               std::string_view folder,
                                               const FileHashMap& fileHashMap);

  static nlohmann::json CommitsToJson(const std::vector<Commit>& commits);

  static std::optional<Repository> RepositoryFromConfigJson(nlohmann::json json);

  static std::optional<std::vector<Commit>> CommitsFromJson(nlohmann::json json);

  static std::optional<std::vector<Commit>> CommitsFromWebJson(nlohmann::json json);

 public:
  static NameFolderMap NameFolderFromJson(nlohmann::json json);

 public:
  static std::optional<Repository> GetRepositoryByFolder(const std::string& folder);

 public:
  static nlohmann::json RepositoryToJson(const Repository& repository);

  static std::optional<Repository> RepositoryFromWebJson(nlohmann::json json);

 public:
  static nlohmann::json CookiesToJson(const cpr::Cookies& cookies);

  static cpr::Cookies CookiesFromJson(nlohmann::json json);

  static cpr::Cookies GetCookiesFromFile();

  static void SaveCookiesInFile(const cpr::Cookies& cookies);
};

#endif  // CODEHUB_JSONSERIALIZER_H
