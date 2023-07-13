#include "filemanager/FilesManager.h"

#include "file/File.h"

FilesManager::FilesManager(const fs::path& folder, types::FileHashMap* fileHashMap,
                           const fs::path& ignoreFile, types::PathSet* ignoredFiles)
    : folder_(folder),
      fileHashMapRef_(*fileHashMap),
      ignoreFileManager_(folder.string(), ignoreFile, ignoredFiles) {}

void FilesManager::Init() { ignoreFileManager_.Init(); }

types::FileHashMap FilesManager::ChangedFiles() const {
  types::FileHashMap changedFiles;
  for (const auto& [file, hash] : fileHashMapRef_) {
    if (!fs::is_directory(file) && !ignoreFileManager_.ShouldBeIgnored(file) &&
        fs::exists(file) && File::CalculateHash(file.string()) != hash) {
      changedFiles.emplace(file, hash);
    }
  }
  return changedFiles;
}

types::FileHashMap FilesManager::RemovedFiles() const {
  types::FileHashMap removedFiles;
  for (const auto& [file, hash] : fileHashMapRef_) {
    if (!fs::exists(file)) {
      removedFiles.emplace(file, hash);
    }
  }
  return removedFiles;
}

types::FileHashMap FilesManager::CreatedFiles() const {
  types::FileHashMap createdFiles;
  for (const auto& file : fs::recursive_directory_iterator(folder_)) {
    if (const auto filename = fs::relative(file);
        !fileHashMapRef_.contains(filename) && !fs::is_directory(filename) &&
        !ignoreFileManager_.ShouldBeIgnored(filename)) {
      createdFiles.emplace(filename, File::CalculateHash(filename));
    }
  }
  return createdFiles;
}

types::FileHashMap FilesManager::CollectFiles() const {
  types::FileHashMap collectedFiles;
  for (const auto& file : fs::recursive_directory_iterator(folder_)) {
    const auto filename = fs::relative(file).string();

    if (!fs::is_directory(filename) && !ignoreFileManager_.ShouldBeIgnored(filename)) {
      collectedFiles.emplace(filename, File::CalculateHash(filename));
    }
  }

  return collectedFiles;
}
