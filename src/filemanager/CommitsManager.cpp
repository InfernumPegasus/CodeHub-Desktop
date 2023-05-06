#include <fstream>
#include "CommitsManager.h"
#include "../serializer/JsonSerializer.h"

CommitsManager::CommitsManager(std::string commitsFile,
                               std::vector<Commit> *commits) :
        commitsFile_(std::move(commitsFile)),
        commitsRef_(*commits) {}

bool CommitsManager::Create() {
    std::ofstream file(commitsFile_);
    return file.is_open();
}

bool CommitsManager::Read() {
    if (!std::filesystem::exists(commitsFile_) ||
        std::filesystem::is_empty(commitsFile_)) {
        return false;
    }

    std::ifstream ifs(commitsFile_.data());
    if (!ifs) return true;

    nlohmann::json j = nlohmann::json::parse(ifs);
    auto readCommits = JsonSerializer::CommitsFromJson(j);
    for (const auto &commit: *readCommits) {
        commitsRef_.push_back(commit);
    }

    return true;
}

void CommitsManager::Update() const {
    std::ofstream ofs(commitsFile_);
    if (!ofs) {
        return;
    }

    auto repoJson = JsonSerializer::CommitsToJson(commitsRef_).dump(2);
    ofs << repoJson;
}
