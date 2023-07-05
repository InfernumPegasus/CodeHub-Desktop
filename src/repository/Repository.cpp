#include "repository/Repository.h"

#include <iostream>
#include <utility>

#include "config/ConfigFiles.h"
#include "filemanager/RestoreFileManager.h"

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder)
    : repositoryName_(std::move(repositoryName)),
      repositoryFolder_(fs::absolute(repositoryFolder)),
      configManager_(repositoryFolder_ / CONFIG_DIRECTORY / CONFIG_FILE, &repositoryName_,
                     &repositoryFolder_, &fileHashMap_),
      commitsManager_(repositoryFolder_ / CONFIG_DIRECTORY / COMMITS_FILE, &commits_),
      filesManager_(repositoryFolder, &fileHashMap_, repositoryFolder_ / IGNORE_FILE,
                    &ignoredFiles_) {}

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder,
                       FileHashMap files)
    : Repository(std::move(repositoryName), repositoryFolder) {
  fileHashMap_ = std::move(files);
}

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder,
                       const std::vector<Commit>& commits)
    : Repository(std::move(repositoryName), repositoryFolder) {
  commits_ = commits;
}

Repository::~Repository() { configManager_.Update(); }

FileHashMap Repository::ChangedFiles() const { return filesManager_.ChangedFiles(); }

FileHashMap Repository::RemovedFiles() const { return filesManager_.RemovedFiles(); }

FileHashMap Repository::CreatedFiles() const { return filesManager_.CreatedFiles(); }

void Repository::DoCommit(std::string_view message) {
  const auto collectedFiles = FilterCollectedFiles(filesManager_.CollectFiles());

  if (collectedFiles.empty()) {
    std::cout << "Nothing to commit!\n";
    return;
  }

  const Commit commit(collectedFiles, message);
  commits_.push_back(commit);
  commitsManager_.Update();
  configManager_.Update();

  SaveCommitFiles(commit);

  const auto statuses = CountFilesStatuses(collectedFiles);
  std::cout << "Commit '" << commit.Message() << "'\n"
            << get<0>(statuses) << " creations, " << get<1>(statuses)
            << " modifications, " << get<2>(statuses) << " deletions\n";
}

std::tuple<int, int, int> Repository::CountFilesStatuses(
    const std::unordered_set<File>& files) {
  int creations{}, modifications{}, deletions{};
  for (const auto& file : files) {
    switch (file.Status()) {
      case FileStatus::Created:
        creations++;
        break;
      case FileStatus::Modified:
        modifications++;
        break;
      case FileStatus::Deleted:
        deletions++;
        break;
      case FileStatus::Unknown:
      default:
        break;
    }
  }
  return {creations, modifications, deletions};
}

void Repository::SaveCommitFiles(const Commit& commit) {
  const auto recoveryFolder =
      fs::path{CONFIG_DIRECTORY} / std::to_string(commit.Checksum());
  RestoreFileManager::CreateRecoveryFolder(recoveryFolder);
  RestoreFileManager::CopyFiles(commit.Files(), fs::current_path(), recoveryFolder);
}

void Repository::RestoreCommitFiles(size_t checksum) {
  RestoreFileManager::CopyRecursive(fs::path{CONFIG_DIRECTORY} / std::to_string(checksum),
                                    fs::current_path());
}

void Repository::InitConfigManager() { configManager_.Init(); }

void Repository::InitCommitsManager() { commitsManager_.Init(); }

void Repository::InitManagers() {
  InitConfigManager();
  InitCommitsManager();
}

const std::string& Repository::Name() const { return repositoryName_; }

const fs::path& Repository::Folder() const { return repositoryFolder_; }

const std::vector<Commit>& Repository::Commits() const { return commits_; }

const FileHashMap& Repository::Map() const { return fileHashMap_; }

std::unordered_set<File> Repository::FilterCollectedFiles(
    const FileHashMap& collectedFiles) {
  std::unordered_set<File> toInsert;
  for (const auto& [filename, hash] : collectedFiles) {
    if (fileHashMap_.contains(filename)) {
      // File exists and modified
      if (fileHashMap_.at(filename) != hash) {
        const auto calcHash = File::CalculateHash(filename);
        // Add file to new commit
        toInsert.emplace(filename, calcHash, FileStatus::Modified);

        // Rewrite modified file's hash
        fileHashMap_.at(filename) = calcHash;
      }
    } else {
      const auto calcHash = File::CalculateHash(filename);
      // File created
      toInsert.emplace(filename, calcHash, FileStatus::Created);

      fileHashMap_.emplace(filename, calcHash);
    }
  }

  auto iter = fileHashMap_.begin();
  while (iter != fileHashMap_.end()) {
    // File deleted
    if (!fs::exists(iter->first)) {
      toInsert.emplace(iter->first, File::CalculateHash(iter->first),
                       FileStatus::Deleted);
      // Erasing file from 'tracked'
      iter = fileHashMap_.erase(iter);
    } else {
      iter++;
    }
  }

  return toInsert;
}
