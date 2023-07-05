#include "filemanager/IgnoreFileManager.h"

#include <filesystem>
#include <fstream>

#include "config/ConfigFiles.h"

IgnoreFileManager::IgnoreFileManager(std::string_view repositoryFolder,
                                     std::string ignoreFile,
                                     std::unordered_set<fs::path>* ignoredFiles)
    : repositoryFolder_(repositoryFolder),
      ignoreFile_(std::move(ignoreFile)),
      ignoredFilesRef_(*ignoredFiles) {}

bool IgnoreFileManager::Create() {
  std::unordered_set<std::string> files;
  for (const auto& file :
       std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
    if (const auto filename = std::filesystem::relative(file).filename().c_str();
        ShouldBeIgnored(filename)) {
      files.insert(filename);
    }
  }

  std::ofstream ofs;
  ofs.open(ignoreFile_);
  if (!ofs) return false;

  for (const auto& file : files) {
    ofs << file << "\n";
  }
  ofs << COMMITS_FILE << "\n";
  ofs << VCS_REPOSITORIES_FILE << "\n";
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

bool IgnoreFileManager::ShouldBeIgnored(std::string_view filename) const {
  return (fs::is_directory(filename)) ||
         (filename.starts_with(".") || filename.starts_with("_")) ||
         ignoredFilesRef_.contains(filename.data());
}
