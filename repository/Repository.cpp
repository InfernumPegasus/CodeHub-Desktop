#include <iostream>
#include <utility>
#include "Repository.h"
#include "../config/Configs.h"
#include "../filemanager/RestoreFileManager.h"

Repository::Repository(std::string repositoryName,
                       const std::string &repositoryFolder) :
        repositoryName_(std::move(repositoryName)),
        repositoryFolder_(std::filesystem::absolute(repositoryFolder)),
        configManager_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_CONFIG_FILE,
                       &repositoryName_,
                       &repositoryFolder_,
                       &fileHashMap_),
        commitsManager_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_COMMITS_FILE,
                        &commits_),
        ignoreFileManager_(repositoryFolder_,
                           repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_IGNORE_FILE,
                           &ignoredFiles_) {}

Repository::Repository(std::string repositoryName,
                       const std::string &repositoryFolder,
                       FileHashMap files) :
        Repository(std::move(repositoryName), repositoryFolder) {
    fileHashMap_ = std::move(files);
}

Repository::Repository(std::string repositoryName,
                       const std::string &repositoryFolder,
                       const std::vector<Commit> &commits) :
        Repository(std::move(repositoryName), repositoryFolder) {
    commits_ = commits;
}

Repository::~Repository() {
    configManager_.Update();
}

FileHashMap Repository::ChangedFiles() const {
    FileHashMap changedFiles;
    for (const auto &[file, hash]: fileHashMap_) {
        if (File::CalculateHash(file) != hash) {
            changedFiles.emplace(file, hash);
        }
    }
    return changedFiles;
}

FileHashMap Repository::CollectFiles() const {
    FileHashMap collectedFiles;

    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        auto filename = std::filesystem::absolute(file).string();

        if (!ignoredFiles_.contains(filename) &&
            !std::filesystem::is_directory(filename)) {
            collectedFiles.emplace(filename, File::CalculateHash(filename));
        }
    }

    return collectedFiles;
}

void Repository::DoCommit(std::string_view message) {
    std::unordered_set<File> collectedFiles =
            FilterCollectedFiles(CollectFiles());

    if (collectedFiles.empty()) {
        std::cout << "Nothing to commit!\n";
        return;
    }

    Commit commit(collectedFiles, message);
    commits_.push_back(commit);
    commitsManager_.Update();
    configManager_.Update();

    SaveCommitFiles(commit);

    std::cout << "Commit created:\n";
    for (const auto &file: collectedFiles) {
        std::cout << "file: '" << file.Name() << "'\n"
                  << "status: " << static_cast<int>(file.Status()) << "\n\n";
    }
}

void Repository::SaveCommitFiles(const Commit &commit) {
    auto recoveryFolder =
            VCS_CONFIG_DIRECTORY + "/" + std::to_string(commit.Checksum());
    RestoreFileManager::CreateRecoveryFolder(recoveryFolder);
    RestoreFileManager::CopyFiles(commit.Files(),
                                  fs::current_path(),
                                  recoveryFolder);
}

void Repository::RestoreCommitFiles(int32_t checksum) {
    RestoreFileManager::CopyRecursive(
            VCS_CONFIG_DIRECTORY + "/" + std::to_string(checksum),
            fs::current_path()
    );
}

void Repository::InitConfigManager() {
    configManager_.Init();
}

void Repository::InitIgnoreManager() {
    ignoreFileManager_.Init();
}

void Repository::InitCommitsManager() {
    commitsManager_.Init();
}

void Repository::InitManagers() {
    configManager_.Init();
    ignoreFileManager_.Init();
    commitsManager_.Init();
}

std::string Repository::Name() const {
    return repositoryName_;
}

std::string Repository::Folder() const {
    return repositoryFolder_;
}

std::vector<Commit> Repository::Commits() const {
    return commits_;
}

Commit Repository::LastCommit() const {
    return commits_.back();
}

FileHashMap Repository::Map() const {
    return fileHashMap_;
}

std::unordered_set<std::string> Repository::MapToFilenames() const {
    std::unordered_set<std::string> files;
    for (const auto &[name, _]: fileHashMap_) {
        files.insert(name);
    }
    return files;
}

std::unordered_set<File> Repository::FilterCollectedFiles(
        const FileHashMap &collectedFiles) {
    std::unordered_set<File> toInsert;
    for (const auto &[filename, hash]: collectedFiles) {
        if (fileHashMap_.contains(filename)) {
            // File exists and modified
            if (fileHashMap_.at(filename) != hash) {
                auto calcHash = File::CalculateHash(filename);
                // Add file to new commit
                toInsert.emplace(filename,
                                 calcHash,
                                 FileStatus::Modified);

                // Rewrite modified file's hash
                fileHashMap_.at(filename) = calcHash;
            }
        } else {
            auto calcHash = File::CalculateHash(filename);
            // File created
            toInsert.emplace(filename,
                             calcHash,
                             FileStatus::Created);

            fileHashMap_.emplace(filename, calcHash);
        }
    }

    auto iter = fileHashMap_.begin();
    while (iter != fileHashMap_.end()) {
        // File deleted
        if (!std::filesystem::exists(iter->first)) {
            toInsert.emplace(iter->first,
                             File::CalculateHash(iter->first),
                             FileStatus::Deleted);
            // Erasing file from 'tracked'
            iter = fileHashMap_.erase(iter);
        } else {
            iter++;
        }
    }

    return toInsert;
}


