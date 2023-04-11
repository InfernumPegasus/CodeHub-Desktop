#include "Commit.h"

Commit::Commit(const std::unordered_set<File> &files,
               std::string_view message) :
        message_(message) {
    for (const auto &file: files) {
        files_.emplace(file);
    }
    checkSum_ = CalculateCheckSum();
}

Commit::Commit(const std::unordered_set<std::string> &files,
               std::string_view message) :
        message_(message) {
    for (const auto &file: files) {
        files_.emplace(file);
    }
    checkSum_ = CalculateCheckSum();
}

Commit::Commit(std::unordered_set<File> files,
               std::string_view message,
               int32_t checkSum) :
        files_(std::move(files)),
        message_(message),
        checkSum_(checkSum) {}

std::ostream &operator<<(std::ostream &os, const Commit &commit) {
    os << "commit " << commit.Checksum()
       << "\n\t" << commit.Message() << "\n";
    return os;
}

std::unordered_set<File> Commit::Files() const {
    return files_;
}

std::string Commit::Message() const {
    return message_;
}

int32_t Commit::Checksum() const {
    return checkSum_;
}

int32_t Commit::CalculateCheckSum() const {
    size_t checkSum = 0;
    auto size = files_.size();
    for (const auto &file: files_) {
        checkSum += file.Hash() + (size << 2) + (size >> 5);
    }
    checkSum += std::hash<std::string>{}(message_);
    return static_cast<int32_t>(checkSum);
}
