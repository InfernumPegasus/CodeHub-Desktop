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

size_t File::CalculateHash(std::string_view filename) {
    return std::filesystem::hash_value(filename);
}
