#include "repository/Repository.h"

#include <iostream>
#include <utility>

#include "config/ConfigFiles.h"
#include "filemanager/RestoreFileManager.h"

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder,
                       std::string branch = "master")
    : repositoryName_(std::move(repositoryName)),
      repositoryFolder_(fs::absolute(repositoryFolder)),
      configManager_(repositoryFolder_ / CONFIG_DIRECTORY / CONFIG_FILE, &repositoryName_,
                     &repositoryFolder_, &trackedFiles_, &currentBranch_),
      commitsManager_(repositoryFolder_ / CONFIG_DIRECTORY / COMMITS_FILE, &commits_),
      filesManager_(repositoryFolder, &trackedFiles_, repositoryFolder_ / IGNORE_FILE,
                    &ignoredFiles_),
      currentBranch_(std::move(branch)) {}

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder,
                       types::FileHashMap files, std::string branch = "master")
    : Repository(std::move(repositoryName), repositoryFolder, std::move(branch)) {
  trackedFiles_ = std::move(files);
}

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder,
                       const types::Commits& commits)
    : Repository(std::move(repositoryName), repositoryFolder) {
  commits_ = commits;
}

Repository::~Repository() { configManager_.Update(); }

types::FileHashMap Repository::ChangedFiles() const {
  return filesManager_.ChangedFiles();
}

types::FileHashMap Repository::RemovedFiles() const {
  return filesManager_.RemovedFiles();
}

types::FileHashMap Repository::CreatedFiles() const {
  return filesManager_.CreatedFiles();
}

types::FilesSet Repository::FilterCollectedFiles(
    const types::FileHashMap& collectedFiles) {
  types::FilesSet files;
  for (const auto& [filename, hash] : collectedFiles) {
    if (trackedFiles_.contains(filename)) {
      // File exists and modified
      if (trackedFiles_.at(filename) != hash) {
        const auto calcHash = File::CalculateHash(filename);
        // Add file to new commit
        files.emplace(filename, calcHash, FileStatus::Modified);

        // Rewrite modified file's hash
        trackedFiles_.at(filename) = calcHash;
      }
    } else {
      const auto calcHash = File::CalculateHash(filename);
      // File created
      files.emplace(filename, calcHash, FileStatus::Created);

      trackedFiles_.emplace(filename, calcHash);
    }
  }

  auto iter = trackedFiles_.begin();
  while (iter != trackedFiles_.end()) {
    // File deleted
    if (!fs::exists(iter->first)) {
      files.emplace(iter->first, File::CalculateHash(iter->first), FileStatus::Deleted);
      // Erasing file from 'tracked'
      iter = trackedFiles_.erase(iter);
    } else {
      iter++;
    }
  }

  return files;
}

void Repository::DoCommit(std::string_view message) {
  const auto filesToCommit = FilterCollectedFiles(filesManager_.CollectFiles());
  if (filesToCommit.empty()) {
    std::cout << "Nothing to commit!\n";
    return;
  }

  const Commit commit(filesToCommit, message);
  commits_.push_back(commit);
  commitsManager_.Update();
  configManager_.Update();

  SaveCommitFiles(commit);

  const auto printFilesStatuses = [&filesToCommit](const auto& message) {
    size_t creations{}, modifications{}, deletions{};
    for (const auto& file : filesToCommit) {
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
    std::cout << "Commit '" << message << "'\n"
              << creations << " creations, " << modifications << " modifications, "
              << deletions << " deletions\n";
  };

  printFilesStatuses(commit.Message());
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

void Repository::InitFilesManager() { filesManager_.Init(); }

void Repository::InitCommitsManager() { commitsManager_.Init(); }

void Repository::InitManagers() {
  InitConfigManager();
  InitFilesManager();
  InitCommitsManager();
}

const std::string& Repository::Name() const { return repositoryName_; }

const fs::path& Repository::Folder() const { return repositoryFolder_; }

const types::Commits& Repository::Commits() const { return commits_; }
