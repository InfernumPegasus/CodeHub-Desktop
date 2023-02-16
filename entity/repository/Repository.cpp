#include <fstream>
#include <ranges>
#include "Repository.h"

using FileHashMap = Repository::FileHashMap;

bool Repository::CreateIgnoreFile() {
    std::ofstream ofs;
    ofs.open(ignoreFile_);
    if (!ofs) {
        return false;
    }

    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        if (auto filename = std::filesystem::absolute(file).filename().string();
                filename.starts_with(".") || filename.starts_with("_")) {
            ofs << std::filesystem::absolute(file).string() << "\n";
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
    if (!ifs) {
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
    if (!ofs && !CreateConfigFile()) {
        std::cout << "Cannot create config folder!\n";
        return;
    }

    auto repoJson = ConfigToJson().dump(2);
    ofs << repoJson;
}

bool Repository::ReadConfigFile() {
    if (!std::filesystem::exists(configFile_)) {
        return false;
    }

    if (std::filesystem::is_empty(configFile_)) {
        return true;
    }

    std::ifstream ifs(configFile_);
    if (ifs) {
        nlohmann::json j = nlohmann::json::parse(ifs);

        repositoryName_ = j["repo_name"];
        repositoryFolder_ = j["repo_folder"];
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
    UpdateCommitsFile();
    UpdateConfigFile();
}

void Repository::Init() {
    if (ReadConfigFile()) {
        std::cout << "Config loaded!\n";
    } else if (CreateConfigFile()) {
        std::cout << "Config file created!\n";
    }

    if (!ReadIgnoreFile()) {
        std::cout << "Creating ignore file!\n";
        CreateIgnoreFile();
        ReadIgnoreFile();
    }

    if (!ReadCommitsFile()) {
        std::cout << "Creating commits file!\n";
        CreateCommitsFile();
        ReadCommitsFile();
    }
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

nlohmann::json Repository::ConfigToJson() const {
    nlohmann::json j;
    j["repo_name"] = repositoryName_;
    j["repo_folder"] = repositoryFolder_;
    j["map"] = fileHashMap_;

    return j;
}

