#include "repository/Repository.h"

#include <iostream>
#include <utility>

#include "filemanager/RestoreFileManager.h"
#include "json/JsonSerializer.h"
#include "log/Logger.h"
#include "utils/ConfigFiles.h"

Repository::Repository(RepositoryConfig config)
    : config_(std::move(config)),
      filesManager_(std::make_unique<FilesManager>(
          config_.repositoryFolder_, &trackedFiles_,
          config_.repositoryFolder_ / IGNORE_FILE, &ignoredFiles_)) {}

Repository::~Repository() {
  const auto repositoryFolder =
      GetHomeDirectory() / VCS_CONFIG_FOLDER / config_.repositoryName_;

  CreateConfigFile(repositoryFolder / REPOSITORY_CONFIG_FILE, config_.ToJson().dump(2));

  nlohmann::json trackedJson;
  trackedJson["tracked_files"] = trackedFiles_;
  CreateConfigFile(repositoryFolder / config_.currentBranch_ / TRACKED_FILE,
                   trackedJson.dump(2));
}

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

  const auto saveFiles = [this](auto&& commit) {
    const auto recoveryFolder = config_.FormCommittedFilesSavePath(commit.Checksum());
    CreateFolder(recoveryFolder);
    RestoreFileManager::CopyFiles(commit.Files(), fs::current_path(), recoveryFolder);
  };
  saveFiles(commit);

  const auto saveCommitsState = [this]() {
    const auto commitsJson = JsonSerializer::CommitsToJson(commits_);
    CreateConfigFile(GetHomeDirectory() / VCS_CONFIG_FOLDER / config_.repositoryName_ /
                         config_.currentBranch_ / COMMITS_FILE,
                     commitsJson.dump(2));
  };
  saveCommitsState();

  const auto printFilesStatuses = [](const auto& message, const auto& files) {
    size_t creations{}, modifications{}, deletions{};
    for (auto&& file : files) {
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

void Repository::Init() {
  filesManager_->Init();
  const auto repoFolder = GetHomeDirectory() / VCS_CONFIG_FOLDER /
                          config_.repositoryName_ / config_.currentBranch_;

  trackedFiles_ = ReadTrackedFromFile(repoFolder / TRACKED_FILE);
  commits_ = ReadCommitsFromFile(repoFolder / COMMITS_FILE);
}

void Repository::AddBranch(const types::Branch& branch) {
  config_.branches_.insert(branch);
}

void Repository::MergeBranch(const types::Branch& mergeWith) {
  config_.CheckBranchExist(mergeWith);

  const auto checkCommitsNotEmpty = [](const types::Commits& commits,
                                       std::string_view branch) {
    if (commits.empty()) {
      throw std::runtime_error(
          fmt::format("There are no commits in [{}] branch", branch));
    }
  };

  checkCommitsNotEmpty(commits_, config_.currentBranch_);

  const auto mergedCommits = ReadCommitsFromFile(
      GetHomeDirectory() / config_.repositoryName_ / mergeWith / COMMITS_FILE);
  checkCommitsNotEmpty(mergedCommits, mergeWith);

  const auto& currentLastCommit = commits_.back();
  const auto& mergedLastCommit = mergedCommits.back();

  // TODO merge two files into one
  const auto mergeCommits = [](const Commit& c1, const Commit& c2) {
    return Commit{types::FilesSet{}, ""};
  };

  const auto mergedCommit = mergeCommits(currentLastCommit, mergedLastCommit);
}

void Repository::ChangeBranch(types::Branch branch) {
  const auto changedFiles = filesManager_->ChangedFiles();
  const auto removedFiles = filesManager_->RemovedFiles();

  // If there are some uncommitted changes, stop creating branch
  if (!changedFiles.empty() || !removedFiles.empty()) {
    throw std::runtime_error(
        fmt::format("Has uncommitted changes: {} modifications, {} deletions",
                    changedFiles.size(), removedFiles.size()));
  }

  config_.currentBranch_ = std::move(branch);

  fmt::print("Switched to branch '{}'\n", config_.currentBranch_);
}

const std::string& Repository::Name() const { return config_.repositoryName_; }

const types::Commits& Repository::Commits() const { return commits_; }

const types::Branch& Repository::CurrentBranch() const { return config_.currentBranch_; }

const types::Branches& Repository::Branches() const { return config_.branches_; }
