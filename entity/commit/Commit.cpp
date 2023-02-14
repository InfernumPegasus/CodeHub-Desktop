#include "Commit.h"

std::set<File> Commit::Files() const {
    return files_;
}

constexpr std::string Commit::Message() const {
    return message_;
}

nlohmann::json Commit::ToJson() const {
    nlohmann::json j;
    std::vector<nlohmann::json> files;
    for (const auto &file: files_) {
        files.push_back(file.ToJson());
    }
    j["files"] = files;
    j["message"] = message_;
    j["checksum"] = checkSum_;
    return j;
}

Commit Commit::FromJson(nlohmann::json json) {
    std::set<File> files;
    for (auto &file: json["files"]) {
        files.insert(File::FromJson(file));
    }
    std::string message = json["message"];
    int64_t checkSum = json["checksum"];
    return {files, message, checkSum};
}
