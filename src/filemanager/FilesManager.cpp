#include "filemanager/FilesManager.h"

#include "file/File.h"

FilesManager::FilesManager(const fs::path& folder, FileHashMap* fileHashMap,
                           const fs::path& ignoreFile,
                           std::unordered_set<fs::path>* ignoredFiles)
    : folder_(folder),
      fileHashMapRef_(*fileHashMap),
      ignoreFileManager_(folder.string(), ignoreFile, ignoredFiles) {}

FileHashMap FilesManager::ChangedFiles() const {
  FileHashMap changedFiles;
  for (const auto& [file, hash] : fileHashMapRef_) {
    if (fs::exists(file) && File::CalculateHash(file.string()) != hash) {
      changedFiles.emplace(file, hash);
    }
  }
  return changedFiles;
}

FileHashMap FilesManager::RemovedFiles() const {
  FileHashMap removedFiles;
  for (const auto& [file, hash] : fileHashMapRef_) {
    if (!fs::exists(file)) {
      removedFiles.emplace(file, hash);
    }
  }
  return removedFiles;
}

FileHashMap FilesManager::CreatedFiles() const {
  FileHashMap createdFiles;
  for (const auto& file : fs::recursive_directory_iterator(folder_)) {
    if (const auto filename = fs::relative(file);
        !fileHashMapRef_.contains(filename) &&
        !ignoreFileManager_.ShouldBeIgnored(filename.c_str())) {
      createdFiles.emplace(filename, File::CalculateHash(filename));
    }
  }
  return createdFiles;
}

FileHashMap FilesManager::CollectFiles() const {
  FileHashMap collectedFiles;

  for (const auto& file : fs::recursive_directory_iterator(folder_)) {
    const auto filename = fs::relative(file).string();

    if (!fs::is_directory(filename) && !ignoreFileManager_.ShouldBeIgnored(filename)) {
      collectedFiles.emplace(filename, File::CalculateHash(filename));
    }
  }

  return collectedFiles;
}
