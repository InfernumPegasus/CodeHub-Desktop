#include <fstream>
#include "CommitsManager.h"
#include "../../serializer/JsonSerializer.h"

CommitsManager::CommitsManager(std::string commitsFile,
                               std::vector<Commit> *commits) :
        commitsFile_(std::move(commitsFile)),
        commitsRef_(*commits) {}

bool CommitsManager::CreateCommitsFile() const {
    std::ofstream file(commitsFile_);
    return file.is_open();
}

void CommitsManager::UpdateCommitsFile() const {
    std::ofstream ofs(commitsFile_);
    if (!ofs && !CreateCommitsFile()) {
        std::cout << "Cannot create commits file!\n";
        return;
    }

    auto repoJson =
            JsonSerializer::CommitsToJson(commitsRef_)
                    .dump(2);
    ofs << repoJson;
}

bool CommitsManager::ReadCommitsFile() {
    if (!std::filesystem::exists(commitsFile_) ||
        std::filesystem::is_empty(commitsFile_)) {
        return false;
    }

    std::ifstream ifs(commitsFile_);
    if (ifs) {
        nlohmann::json j = nlohmann::json::parse(ifs);
        auto commits = JsonSerializer::CommitsFromJson(j);
        for (const auto &commit: commits) {
            commitsRef_.push_back(commit);
        }

        return true;
    }

    return true;
}
