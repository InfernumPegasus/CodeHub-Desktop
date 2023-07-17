#ifndef CODEHUB_CONFIGFILES_H
#define CODEHUB_CONFIGFILES_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

// clang-format off
const inline fs::path CONFIG_DIRECTORY      = ".codehub_config";

const inline fs::path CONFIG_FILE           = ".repo_info.json";
const inline fs::path COMMITS_FILE          = ".commits.json";
const inline fs::path IGNORE_FILE           = ".ignore";

const inline fs::path VCS_CONFIG_FOLDER     = ".codehub_app";
const inline fs::path VCS_REPOSITORIES_FILE = "repos.json";
const inline fs::path VCS_USER_FILE         = "user.json";

const inline fs::path COOKIES_FILE          = ".cookies.json";
const inline fs::path CONFIG_COOKIES_FILE   = CONFIG_DIRECTORY / COOKIES_FILE;
// clang-format on

#endif  // CODEHUB_CONFIGFILES_H
