#include <fstream>
#include <filesystem>
#include "IgnoreFileManager.h"

IgnoreFileManager::IgnoreFileManager(
        std::string_view repositoryFolder,
        std::string ignoreFile,
        std::set<std::string> *ignoredFiles) :
        repositoryFolder_(repositoryFolder),
        ignoreFile_(std::move(ignoreFile)),
        ignoredFilesRef_(*ignoredFiles) {}

bool IgnoreFileManager::CreateIgnoreFile() const {
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

bool IgnoreFileManager::ReadIgnoreFile() {
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

        ignoredFilesRef_.emplace(readFilename);
        if (std::filesystem::is_directory(readFilename)) {
            for (auto &file:
                    std::filesystem::recursive_directory_iterator(readFilename)) {
                auto filename = std::filesystem::absolute(file).string();
                ignoredFilesRef_.insert(filename);
            }
        }
    }

    return true;
}
