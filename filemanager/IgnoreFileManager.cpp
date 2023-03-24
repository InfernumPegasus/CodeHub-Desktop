#include <fstream>
#include <filesystem>
#include "IgnoreFileManager.h"

IgnoreFileManager::IgnoreFileManager(
        std::string_view repositoryFolder,
        std::string ignoreFile,
        std::unordered_set<std::string> *ignoredFiles) :
        repositoryFolder_(repositoryFolder),
        ignoreFile_(std::move(ignoreFile)),
        ignoredFilesRef_(*ignoredFiles) {}

bool IgnoreFileManager::CreateIgnoreFile() const {
    std::unordered_set<std::string> files;
    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        if (auto filename = std::filesystem::absolute(file).filename().string();
                filename.starts_with(".") || filename.starts_with("_")) {
            files.insert(std::filesystem::absolute(file).string());
        }
    }

    std::ofstream ofs;
    ofs.open(ignoreFile_.data());
    if (!ofs) return false;

    for (const auto &file: files) {
        ofs << file << "\n";
    }

    return true;
}

bool IgnoreFileManager::ReadIgnoreFile() {
    std::ifstream ifs(ignoreFile_.data());
    if (!ifs) return false;

    std::string readFilename;
    while (ifs.good()) {
        std::getline(ifs, readFilename);
        if (std::filesystem::exists(readFilename)) {
            ignoredFilesRef_.emplace(readFilename);
            if (!std::filesystem::is_directory(readFilename)) continue;

            for (auto &file:
                    std::filesystem::recursive_directory_iterator(readFilename)) {
                auto filename = std::filesystem::absolute(file).string();
                ignoredFilesRef_.insert(filename);
            }
        }
    }
    return true;
}
