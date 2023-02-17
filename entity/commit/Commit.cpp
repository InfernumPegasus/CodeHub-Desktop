#include "Commit.h"

std::set<File> Commit::Files() const {
    return files_;
}

std::string Commit::Message() const {
    return message_;
}

size_t Commit::Checksum() const {
    return checkSum_;
}

size_t Commit::CalculateCheckSum() const {
    size_t checkSum = 0;
    auto size = files_.size();
    for (const auto &file: files_) {
        checkSum += file.Hash() + (size << 2) + (size >> 5);
    }
    checkSum += std::hash<std::string>{}(message_);
    return checkSum;
}
