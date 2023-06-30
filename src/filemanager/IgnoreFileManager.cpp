#include <fstream>
#include <filesystem>
#include "filemanager/IgnoreFileManager.h"
#include "config/ConfigFiles.h"

IgnoreFileManager::IgnoreFileManager(
        std::string_view repositoryFolder,
        std::string ignoreFile,
        std::unordered_set<std::string> *ignoredFiles) :
        repositoryFolder_(repositoryFolder),
        ignoreFile_(std::move(ignoreFile)),
        ignoredFilesRef_(*ignoredFiles) {}

bool IgnoreFileManager::Create() {
    std::unordered_set<std::string> files;
    for (auto &file:
            std::filesystem::recursive_directory_iterator(repositoryFolder_)) {
        if (auto filename = std::filesystem::relative(file).filename().string();
                filename.starts_with(".") || filename.starts_with("_")) {
            files.insert(filename);
        }
    }

    std::ofstream ofs;
    ofs.open(ignoreFile_);
    if (!ofs) return false;

    for (const auto &file: files) {
        ofs << file << "\n";
    }
    ofs << COMMITS_FILE << "\n";
    ofs << VCS_REPOSITORIES_FILE << "\n";
    ofs << VCS_USER_FILE << "\n";

    return true;
}

bool IgnoreFileManager::Read() {
    std::ifstream ifs(ignoreFile_);
    if (!ifs) return false;

    std::string readFilename;
    while (ifs.good()) {
        std::getline(ifs, readFilename);
        if (!std::filesystem::exists(readFilename)) continue;

        ignoredFilesRef_.emplace(readFilename);
        if (!std::filesystem::is_directory(readFilename)) continue;

        for (auto &file:
                std::filesystem::recursive_directory_iterator(readFilename)) {
            auto filename = std::filesystem::relative(file).string();
            ignoredFilesRef_.insert(filename);
        }
    }
    return true;
}
