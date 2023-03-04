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
    std::set<std::string> files;
    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        if (auto filename = std::filesystem::absolute(file).filename().string();
                filename.starts_with(".") || filename.starts_with("_")) {
            files.insert(std::filesystem::absolute(file).string());
        }
    }

    return CreateIgnoreFile(ignoreFile_, files);
}

bool IgnoreFileManager::ReadIgnoreFile() {
    return ReadIgnoreFile(ignoreFile_, ignoredFilesRef_);
}

bool IgnoreFileManager::CreateIgnoreFile(
        std::string_view path,
        const std::set<std::string> &files) {
    std::ofstream ofs;
    ofs.open(path.data());
    if (!ofs) {
        return false;
    }

    for (const auto &file: files) {
        ofs << file << "\n";
    }

    return true;
}

bool IgnoreFileManager::ReadIgnoreFile(
        std::string_view path,
        std::set<std::string> &files) {
    std::ifstream ifs(path.data());
    if (ifs) {
        std::string readFilename;
        while (ifs.good()) {
            std::getline(ifs, readFilename);
            if (!std::filesystem::exists(readFilename)) {
                continue;
            }

            files.emplace(readFilename);
            if (std::filesystem::is_directory(readFilename)) {
                for (auto &file:
                        std::filesystem::recursive_directory_iterator(readFilename)) {
                    auto filename = std::filesystem::absolute(file).string();
                    files.insert(filename);
                }
            }
        }
        return true;
    }
    return false;
}
