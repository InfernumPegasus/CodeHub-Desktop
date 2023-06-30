#ifndef CODEHUB_CONFIGFILES_H
#define CODEHUB_CONFIGFILES_H

#include <string>

constexpr std::string CONFIG_DIRECTORY = ".codehub_config";
constexpr std::string CONFIG_FILE = ".repo_info.json";
constexpr std::string COMMITS_FILE = ".commits.json";
constexpr std::string IGNORE_FILE = ".ignore";

constexpr std::string VCS_REPOSITORIES_FILE = "repos.json";

constexpr std::string VCS_USER_FILE = "user.json";

constexpr std::string COOKIES_FILE = ".cookies.json";
const std::string CONFIG_COOKIES_FILE = CONFIG_DIRECTORY + "/" + COOKIES_FILE;

#endif //CODEHUB_CONFIGFILES_H
