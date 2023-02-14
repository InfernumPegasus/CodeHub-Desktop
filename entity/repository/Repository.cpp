#include <fstream>
#include <ranges>
#include "Repository.h"

using FileHashMap = Repository::FileHashMap;

bool Repository::CreateIgnoreFile() {
    std::ofstream ofs;
    ofs.open(ignoreFile_, std::ios_base::ate);
    if (!ofs.is_open()) {
        return false;
    }

    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        auto filename = std::filesystem::absolute(file).filename().string();
        if (filename.starts_with(".")) {
            auto absolute = std::filesystem::absolute(file).string() + "\n";
            ofs.write(absolute.c_str(), static_cast<long>(absolute.length()));
        }
    }

    return true;
}

bool Repository::ReadIgnoreFile() {
    if (!std::filesystem::exists(ignoreFile_) ||
        std::filesystem::is_empty(ignoreFile_)) {
        return false;
    }

    std::ifstream ifs(ignoreFile_);
    if (!ifs.is_open()) {
        return false;
    }

    std::string readFilename;
    while (ifs.good()) {
        std::getline(ifs, readFilename);
        if (!std::filesystem::exists(readFilename)) {
            continue;
        }

        ignoredFiles_.emplace(readFilename);
        if (std::filesystem::is_directory(readFilename)) {
            for (auto &file:
                    std::filesystem::recursive_directory_iterator(readFilename)) {
                auto filename = std::filesystem::absolute(file).string();
                ignoredFiles_.insert(filename);
            }
        }
    }

    return true;
}

void Repository::UpdateIgnoreFile() {
    std::ofstream ofs;
    ofs.open(ignoreFile_);
    if (ofs.is_open()) {
        for (const auto &item: ignoredFiles_) {
            std::string file = item + "\n";
            ofs.write(file.c_str(), (long) file.length());
        }
    }
}

bool Repository::CreateConfigFile() const {
    std::string configDirectory = repositoryFolder_ + "/" + VCS_CONFIG_DIRECTORY;
    std::ofstream file;
    if ((std::filesystem::exists(configDirectory) &&
         !std::filesystem::exists(configFile_)) ||
        std::filesystem::create_directory(configDirectory)) {}
    file.open(configFile_);
    return file.is_open();
}

void Repository::UpdateConfigFile() const {
    std::ofstream ofs(configFile_);
    if (!ofs.is_open() && !CreateConfigFile()) {
        std::cout << "Cannot create config folder!";
        return;
    }

    auto repoJson = ToJson().dump(2);
    ofs.write(repoJson.c_str(), static_cast<long>(repoJson.length()));
}

bool Repository::LoadConfigFile() {
    if (!std::filesystem::exists(configFile_)) {
        return false;
    }

    if (std::filesystem::is_empty(configFile_)) {
        return true;
    }

    std::ifstream ofs(configFile_);
    if (ofs.is_open()) {
        nlohmann::json j = nlohmann::json::parse(ofs);
        std::vector<Commit> commitsVector;

        for (auto &commit: j["commits"]) {
            commitsVector.emplace_back(Commit::FromJson(commit));
        }

        repositoryName_ = j["repo_name"];
        repositoryFolder_ = j["repo_folder"];
        commits_ = commitsVector;
        fileHashMap_ = j["map"];

        return true;
    }

    return false;
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
    using std::cout;
    using std::endl;

    std::set<File> toInsert;
    auto collectedFiles = CollectFiles();
    for (const auto &[filename, hash]: collectedFiles) {
        // File does not exist
        if (!std::filesystem::exists(filename)) {
            // File exists only in Map and doesn't exist in FS
            if (fileHashMap_.contains(filename)) {
                toInsert.emplace(filename,
                                 File::LastWriteTimeString(filename),
                                 File::CalculateHash(filename),
                                 FileStatus::Deleted);

                fileHashMap_.erase(filename);
            }
            // File exists
        } else if (fileHashMap_.contains(filename)) {
            // File exists and modified
            if (fileHashMap_.at(filename) != hash) {
                auto calcHash = File::CalculateHash(filename);
                toInsert.emplace(filename,
                                 File::LastWriteTimeString(filename),
                                 calcHash,
                                 FileStatus::Modified);

                fileHashMap_.at(filename) = calcHash;
            }
        } else {
            auto calcHash = File::CalculateHash(filename);
            toInsert.emplace(filename,
                             File::LastWriteTimeString(filename),
                             calcHash,
                             FileStatus::Created);

            fileHashMap_.emplace(filename, calcHash);
        }
    }
    if (toInsert.empty()) {
        std::cout << "Nothing to commit!\n";
        return;
    }

    commits_.emplace_back(toInsert, message.data());
    UpdateConfigFile();
}

void Repository::Init() {
    if (LoadConfigFile()) {
        std::cout << "Config loaded!\n";
    } else if (CreateConfigFile()) {
        std::cout << "Config file created!\n";
    }

    if (!ReadIgnoreFile()) {
        std::cout << "Creating ignore file!";
        CreateIgnoreFile();
        ReadIgnoreFile();
    }
    UpdateIgnoreFile();
}

constexpr std::string Repository::Name() const {
    return repositoryName_;
}

constexpr std::string Repository::Folder() const {
    return repositoryFolder_;
}

constexpr std::vector<Commit> Repository::Commits() const {
    return commits_;
}

nlohmann::json Repository::ToJson() const {
    nlohmann::json j;
    std::vector<nlohmann::json> commitsJson;
    std::for_each(commits_.begin(), commits_.end(),
                  [&](const Commit &commit) {
                      commitsJson.push_back(commit.ToJson());
                  });

    j["repo_name"] = repositoryName_;
    j["repo_folder"] = repositoryFolder_;
    j["commits"] = commitsJson;
    j["map"] = fileHashMap_;

    return j;
}

Repository Repository::FromJson(nlohmann::json json) {
    std::string repoName = json["repo_name"];
    std::string repoFolder = json["repo_folder"];
    std::vector<nlohmann::json> commitsJson = json["commits"];
    FileHashMap fileHashMap = json["map"];
    std::vector<Commit> commits;
    std::for_each(commitsJson.begin(), commitsJson.end(),
                  [&](nlohmann::json &commit) {
                      commits.push_back(Commit::FromJson(commit));
                  });

    return {repoName, repoFolder, commits, fileHashMap};

}
