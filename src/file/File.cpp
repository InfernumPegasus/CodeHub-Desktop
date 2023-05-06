#include <fstream>
#include <utility>
#include "File.h"

File::File(std::string filename,
           size_t hash,
           FileStatus status) :
        name_(std::move(filename)),
        hash_(hash),
        status_(status) {}

File::File(std::string filename) :
        name_(std::move(filename)),
        hash_(CalculateHash(name_)),
        status_(FileStatus::Unknown) {}

const std::string &File::Name() const {
    return name_;
}

size_t File::Hash() const {
    return hash_;
}

FileStatus File::Status() const {
    return status_;
}

bool File::operator<(const File &rhs) const {
    return hash_ < rhs.hash_;
}

std::vector<char> File::LoadContent(std::string_view filename) {
    std::vector<char> content;
    std::ifstream ifs(filename.data());

    return {
            std::istreambuf_iterator<char>(ifs),
            std::istreambuf_iterator<char>()
    };
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
