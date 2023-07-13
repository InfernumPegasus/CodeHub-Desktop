#include "commit/Commit.h"

Commit::Commit(types::FilesSet files, std::string_view message)
    : message_(message), files_{std::move(files)}, checkSum_(CalculateCheckSum()) {}

Commit::Commit(const std::unordered_set<std::string>& files, std::string_view message)
    : message_(message) {
  for (const auto& file : files) {
    files_.emplace(file);
  }
  checkSum_ = CalculateCheckSum();
}

Commit::Commit(types::FilesSet files, std::string_view message, size_t checkSum)
    : files_(std::move(files)), message_(message), checkSum_(checkSum) {}

std::ostream& operator<<(std::ostream& os, const Commit& commit) {
  os << "commit " << commit.Checksum() << "\n\t" << commit.Message() << "\n";
  return os;
}

const types::FilesSet& Commit::Files() const { return files_; }

const std::string& Commit::Message() const { return message_; }

size_t Commit::Checksum() const { return checkSum_; }

size_t Commit::CalculateCheckSum() const {
  size_t checkSum = 0;
  const auto size = files_.size();
  for (const auto& file : files_) {
    checkSum += file.Hash() + (size << 2) + (size >> 5);
  }
  checkSum += std::hash<decltype(message_)>{}(message_);
  return checkSum;
}
