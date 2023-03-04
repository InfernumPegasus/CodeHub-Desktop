#include <fstream>
#include "CommitsManager.h"
#include "../serializer/JsonSerializer.h"

CommitsManager::CommitsManager(std::string commitsFile,
                               std::vector<Commit> *commits) :
        commitsFile_(std::move(commitsFile)),
        commitsRef_(*commits) {}

bool CommitsManager::CreateCommitsFile() const {
    return CreateCommitsFile(commitsFile_);
}

void CommitsManager::UpdateCommitsFile() const {
    UpdateCommitsFile(commitsFile_, commitsRef_);
}

bool CommitsManager::ReadCommitsFile() {
    return ReadCommitsFile(commitsFile_, commitsRef_);
}

bool CommitsManager::CreateCommitsFile(std::string_view path) {
    std::ofstream file(path.data());
    return file.is_open();
}

void CommitsManager::UpdateCommitsFile(
        std::string_view path,
        const std::vector<Commit> &commits) {
    std::ofstream ofs(path.data());
    if (!ofs) {
        std::cout << "Cannot create commits file!\n";
        return;
    }

    auto repoJson = JsonSerializer::CommitsToJson(commits).dump(2);
    ofs << repoJson;
}

bool CommitsManager::ReadCommitsFile(
        std::string_view path,
        std::vector<Commit> &commits) {
    if (!std::filesystem::exists(path.data()) ||
        std::filesystem::is_empty(path.data())) {
        return false;
    }

    std::ifstream ifs(path.data());
    if (ifs) {
        nlohmann::json j = nlohmann::json::parse(ifs);
        auto readCommits = JsonSerializer::CommitsFromJson(j);
        for (const auto &commit: readCommits) {
            commits.push_back(commit);
        }

        return true;
    }

    return true;
}
