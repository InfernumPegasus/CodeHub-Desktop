#include "filemanager/IgnoreFileManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

#include "utils/ConfigFiles.h"

IgnoreFileManager::IgnoreFileManager(fs::path repositoryFolder, fs::path ignoreFile,
                                     types::PathSet* ignoredFilesRef)
    : repositoryFolder_(std::move(repositoryFolder)),
      ignoreFile_(std::move(ignoreFile)),
      ignoredFilesRef_(*ignoredFilesRef) {}

bool IgnoreFileManager::Create() {
  std::ofstream ofs(ignoreFile_);
  if (!ofs) return false;

  std::unordered_set<std::string> files;
  for (const auto& file :
       std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
    if (const auto filename = std::filesystem::relative(file).filename().string();
        filename.starts_with(".") || filename.starts_with("_")) {
      files.insert(filename);
    }
  }

  for (const auto& file : files) {
    ofs << file << "\n";
  }
  ofs << COMMITS_FILE << "\n";
  ofs << VCS_USER_FILE << "\n";

  return true;
}

bool IgnoreFileManager::Read() {
  std::ifstream ifs(ignoreFile_);
  if (!ifs) return false;

  std::string readFilename;
  while (ifs.good()) {
    std::getline(ifs, readFilename);
    if (!std::filesystem::exists(readFilename)) continue;

    ignoredFilesRef_.emplace(readFilename);
    if (!std::filesystem::is_directory(readFilename)) continue;

    for (const auto& file : std::filesystem::recursive_directory_iterator(readFilename)) {
      const auto filename = std::filesystem::relative(file).string();
      ignoredFilesRef_.insert(filename);
    }
  }
  return true;
}

bool IgnoreFileManager::ShouldBeIgnored(const std::string& filename) const {
  return filename.starts_with(".") || filename.starts_with("_") ||
         ignoredFilesRef_.contains(filename);
}
