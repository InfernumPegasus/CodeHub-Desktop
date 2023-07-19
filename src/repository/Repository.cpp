#include "repository/Repository.h"

#include <iostream>
#include <utility>

#include "config/ConfigFiles.h"
#include "filemanager/RestoreFileManager.h"
#include "log/Logger.h"

Repository::Repository(std::string repositoryName, const fs::path& repositoryFolder,
                       types::Branch branch = "master")
    : repositoryName_(std::move(repositoryName)),
      repositoryFolder_(fs::absolute(repositoryFolder)),
      currentBranch_(std::move(branch)),
      configManager_(std::make_unique<RepositoryConfigManager>(
          repositoryFolder_ / CONFIG_DIRECTORY / CONFIG_FILE, &repositoryName_,
          &repositoryFolder_, &trackedFiles_, &currentBranch_)),
      commitsManager_(std::make_unique<CommitsManager>(
          RepositoryConfigManager::GetHomeDirectory() / VCS_CONFIG_FOLDER /
              repositoryName_ / currentBranch_,
          &commits_)),
      filesManager_(std::make_unique<FilesManager>(repositoryFolder_, &trackedFiles_,
                                                   repositoryFolder_ / IGNORE_FILE,
                                                   &ignoredFiles_)) {
  RestoreFileManager::CreateFolder(RepositoryConfigManager::GetHomeDirectory() /
                                   VCS_CONFIG_FOLDER / repositoryName_ / currentBranch_);
}

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

Repository::~Repository() { configManager_->Update(); }

types::FileHashMap Repository::ChangedFiles() const {
  return filesManager_->ChangedFiles();
}

types::FileHashMap Repository::RemovedFiles() const {
  return filesManager_->RemovedFiles();
}

types::FileHashMap Repository::CreatedFiles() const {
  return filesManager_->CreatedFiles();
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

void Repository::DoCommit(const std::string& message) {
  const auto filesToCommit = FilterCollectedFiles(filesManager_->CollectFiles());
  if (filesToCommit.empty()) {
    std::cout << "Nothing to commit!\n";
    return;
  }

  const Commit commit(filesToCommit, message);
  commits_.push_back(commit);
  commitsManager_->Update();
  configManager_->Update();

  SaveCommitFiles(commit);

  const auto printFilesStatuses = [](const auto& message, const auto& files) {
    size_t creations{}, modifications{}, deletions{};
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
    fmt::print("Commit '{}'\n{} creations, {} modifications, {} deletions\n", message,
               creations, modifications, deletions);
  };

  printFilesStatuses(commit.Message(), filesToCommit);
}

void Repository::SaveCommitFiles(const Commit& commit) {
  const auto recoveryFolder = IFileManager::GetHomeDirectory() / VCS_CONFIG_FOLDER /
                              repositoryName_ / currentBranch_ /
                              std::to_string(commit.Checksum());
  RestoreFileManager::CreateFolder(recoveryFolder);
  RestoreFileManager::CopyFiles(commit.Files(), fs::current_path(), recoveryFolder);
  logging::Log(LOG_NOTICE,
               fmt::format("Commit files saved in '{}'", recoveryFolder.c_str()));
}

void Repository::InitConfigManager() { configManager_->Init(); }

void Repository::InitFilesManager() { filesManager_->Init(); }

void Repository::InitCommitsManager() { commitsManager_->Init(); }

void Repository::InitManagers() {
  InitConfigManager();
  logging::Log(LOG_NOTICE, "Repository::InitConfigManager success");
  InitFilesManager();
  logging::Log(LOG_NOTICE, "Repository::InitFilesManager success");
  InitCommitsManager();
  logging::Log(LOG_NOTICE, "Repository::InitCommitsManager success");
}

void Repository::ChangeBranch(std::string branch) {
  const auto changedFiles = filesManager_->ChangedFiles();
  const auto removedFiles = filesManager_->RemovedFiles();
  const auto createdFiles = filesManager_->CreatedFiles();

  if (!changedFiles.empty() || !removedFiles.empty() || !createdFiles.empty()) {
    throw std::runtime_error(fmt::format(
        "Has uncommitted changes: {} creations, {} modifications, {} deletions",
        createdFiles.size(), changedFiles.size(), removedFiles.size()));
  }

  const auto currentStateFolder{IFileManager::GetHomeDirectory() / VCS_CONFIG_FOLDER /
                                repositoryName_ / currentBranch_};
  logging::Log(LOG_WARNING,
               fmt::format("Saving files from '{}' in '{}'", fs::current_path().c_str(),
                           currentStateFolder.c_str()));
  //  RestoreFileManager::CreateFolder(currentStateFolder);
  //  RestoreFileManager::CopyFolderRecursive(fs::current_path(), currentStateFolder);

  fmt::print("Current branch: {}\n", branch);
  //  currentBranch_ = std::move(branch);
}

const std::string& Repository::Name() const { return repositoryName_; }

const fs::path& Repository::Folder() const { return repositoryFolder_; }

const types::Commits& Repository::Commits() const { return commits_; }

const std::string& Repository::CurrentBranch() const { return currentBranch_; }
