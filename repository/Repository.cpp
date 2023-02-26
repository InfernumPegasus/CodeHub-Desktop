#include "Repository.h"

using FileHashMap = Repository::FileHashMap;

Repository::Repository(std::string_view repositoryName,
                       std::string_view repositoryFolder) :
        repositoryName_(repositoryName),
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

Repository::Repository(std::string_view repositoryName,
                       std::string_view repositoryFolder,
                       FileHashMap files) :
        repositoryName_(repositoryName),
        repositoryFolder_(repositoryFolder),
        fileHashMap_(std::move(files)),
        configManager_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_CONFIG_FILE,
                       &repositoryName_,
                       &repositoryFolder_,
                       &fileHashMap_),
        commitsManager_(repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_COMMITS_FILE,
                        &commits_),
        ignoreFileManager_(repositoryFolder_,
                           repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY + "/" + VCS_IGNORE_FILE,
                           &ignoredFiles_) {}

Repository::~Repository() {
    configManager_.UpdateConfigFile();
    commitsManager_.UpdateCommitsFile();
}

size_t Repository::ChangedFilesAmount() const {
    return CollectFiles().size();
}

FileHashMap Repository::CollectFiles() const {
    FileHashMap collectedFiles;

    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        auto filename = std::filesystem::absolute(file).string();

        if (ignoredFiles_.contains(filename) ||
            std::filesystem::is_directory(filename)) {
            continue;
        }

        collectedFiles.emplace(filename, File::CalculateHash(filename));
    }

    return collectedFiles;
}

void Repository::DoCommit(std::string_view message) {
    std::set<File> toInsert;
    auto collectedFiles = CollectFiles();
    for (const auto &[filename, hash]: collectedFiles) {
        if (fileHashMap_.contains(filename)) {
            // File exists and modified
            if (fileHashMap_.at(filename) != hash) {
                auto calcHash = File::CalculateHash(filename);
                toInsert.emplace(filename,
                                 calcHash,
                                 FileStatus::Modified);

                fileHashMap_.at(filename) = calcHash;
            }
        } else {
            auto calcHash = File::CalculateHash(filename);
            toInsert.emplace(filename,
                             calcHash,
                             FileStatus::Created);

            fileHashMap_.emplace(filename, calcHash);
        }
    }

    auto iter = fileHashMap_.begin();
    while (iter != fileHashMap_.end()) {
        if (!std::filesystem::exists(iter->first)) {
            toInsert.emplace(iter->first,
                             File::CalculateHash(iter->first),
                             FileStatus::Deleted);
            iter = fileHashMap_.erase(iter);
        } else {
            iter++;
        }
    }

    if (toInsert.empty()) {
        std::cout << "Nothing to commit!\n";
        return;
    }

    commits_.emplace_back(toInsert, message.data());
    commitsManager_.UpdateCommitsFile();
    configManager_.UpdateConfigFile();
}

void Repository::Init() {
    if (configManager_.ReadConfigFile()) {
        std::cout << "Config loaded!\n";
    } else if (configManager_.CreateConfigFile()) {
        std::cout << "Config file created!\n";
    }

    if (!ignoreFileManager_.ReadIgnoreFile()) {
        if (ignoreFileManager_.CreateIgnoreFile()) {
            std::cout << "Ignore file created!\n";
            ignoreFileManager_.ReadIgnoreFile();
        }
    }

    if (!commitsManager_.ReadCommitsFile()) {
        if (commitsManager_.CreateCommitsFile()) {
            std::cout << "Commits file created!\n";
            commitsManager_.ReadCommitsFile();
        }
    }
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

FileHashMap Repository::Map() const {
    return fileHashMap_;
}

bool Repository::operator<(const Repository &rhs) const {
    return repositoryName_ < rhs.repositoryName_;
}
