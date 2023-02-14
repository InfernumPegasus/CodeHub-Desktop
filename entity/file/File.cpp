#include <fstream>
#include "File.h"

std::string File::Name() const {
    return name_;
}

size_t File::Hash() const {
    return hash_;
}

FileStatus File::Status() const {
    return status_;
}

constexpr bool File::operator==(const File &rhs) const {
    return name_ == rhs.name_ &&
           hash_ == rhs.hash_ &&
           status_ == rhs.status_;
}

bool File::operator<(const File &rhs) const {
    return hash_ < rhs.hash_;
}

nlohmann::json File::ToJson() const {
    nlohmann::json j;
    j["name"] = name_;
    j["hash"] = hash_;
    j["status"] = status_;
    return j;
}

File File::FromJson(nlohmann::json json) {
    std::string name = json["name"];
    size_t hash = json["hash"];
    FileStatus status = json["status"];
    return {name, hash, status};
}

// TODO улучшить алгоритм
std::vector<char> File::LoadContent(std::string_view filename) {
    std::vector<char> content;
    std::ifstream ifs(filename.data());
    if (ifs) {
        char c;
        while (ifs.good()) {
            ifs.read(&c, sizeof(char));
            content.push_back(c);
        }
    }
    return content;
}

size_t File::CalculateHash(std::string_view filename) {
    auto content = LoadContent(filename);
    auto res = content.size();
    std::string file = filename.data();
    for (auto &c: content) {
        res ^= c + std::hash<std::string>{}(file) +
               0x9e3779b9 + (res << 6) + (res >> 2);
    }
    return res;
}
