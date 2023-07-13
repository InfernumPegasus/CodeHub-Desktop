#ifndef CODEHUB_CONFIGFILES_H
#define CODEHUB_CONFIGFILES_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

// clang-format off
constexpr std::string CONFIG_DIRECTORY      = ".codehub_config";
constexpr std::string CONFIG_FILE           = ".repo_info.json";
constexpr std::string COMMITS_FILE          = ".commits.json";
constexpr std::string IGNORE_FILE           = ".ignore";

constexpr std::string VCS_REPOSITORIES_FILE = "repos.json";

constexpr std::string VCS_USER_FILE         = "user.json";

constexpr std::string COOKIES_FILE          = ".cookies.json";
const fs::path CONFIG_COOKIES_FILE          = fs::path{CONFIG_DIRECTORY} / COOKIES_FILE;
// clang-format on

#endif  // CODEHUB_CONFIGFILES_H
