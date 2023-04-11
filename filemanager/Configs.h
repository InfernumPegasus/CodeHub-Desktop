#ifndef CODEHUB_CONFIGS_H
#define CODEHUB_CONFIGS_H

#include <string>

static constexpr std::string VCS_CONFIG_DIRECTORY = ".codehub_config";
static constexpr std::string VCS_CONFIG_FILE = ".repo_info.json";
static constexpr std::string VCS_COMMITS_FILE = ".commits.json";
static constexpr std::string VCS_IGNORE_FILE = ".ignore";

static constexpr std::string VCS_REPOSITORIES_FILE = "repos.json";

static constexpr std::string USER_FILE = "user.json";
static const std::string CONFIG_USER_FILE = VCS_CONFIG_DIRECTORY + "/" + USER_FILE;

static constexpr std::string COOKIES_FILE = ".cookies.json";
static const std::string CONFIG_COOKIES_FILE = VCS_CONFIG_DIRECTORY + "/" + COOKIES_FILE;

#endif //CODEHUB_CONFIGS_H
