#ifndef CODEHUB_TYPES_H
#define CODEHUB_TYPES_H

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include "file/File.h"

namespace fs = std::filesystem;

namespace types {
using NameFolderMap = std::unordered_map<std::string, fs::path>;

using FileHashMap = std::unordered_map<fs::path, size_t>;

using FilesSet = std::unordered_set<File>;

using PathSet = std::unordered_set<fs::path>;

using Branch = std::string;
using Branches = std::unordered_set<Branch>;
}  // namespace types

#endif  // CODEHUB_TYPES_H
