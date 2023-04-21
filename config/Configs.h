#ifndef CODEHUB_CONFIGS_H
#define CODEHUB_CONFIGS_H

#include <string>

constexpr std::string VCS_CONFIG_DIRECTORY = ".codehub_config";
constexpr std::string VCS_CONFIG_FILE = ".repo_info.json";
constexpr std::string VCS_COMMITS_FILE = ".commits.json";
constexpr std::string VCS_IGNORE_FILE = ".ignore";

constexpr std::string VCS_REPOSITORIES_FILE = "repos.json";

constexpr std::string USER_FILE = "user.json";
const std::string CONFIG_USER_FILE = VCS_CONFIG_DIRECTORY + "/" + USER_FILE;

constexpr std::string COOKIES_FILE = ".cookies.json";
const std::string CONFIG_COOKIES_FILE = VCS_CONFIG_DIRECTORY + "/" + COOKIES_FILE;

#endif //CODEHUB_CONFIGS_H
