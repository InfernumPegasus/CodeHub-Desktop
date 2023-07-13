#include "file/File.h"

#include <fstream>
#include <utility>

File::File(fs::path filename, size_t hash, FileStatus status)
    : name_(std::move(filename)), hash_(hash), status_(status) {}

File::File(fs::path filename)
    : name_(std::move(filename)),
      hash_(CalculateHash(name_)),
      status_(FileStatus::Unknown) {}

const fs::path& File::Name() const { return name_; }

size_t File::Hash() const { return hash_; }

FileStatus File::Status() const { return status_; }

bool File::operator<(const File& rhs) const { return hash_ < rhs.hash_; }

std::vector<char> File::LoadContent(const fs::path& filename) {
  std::ifstream ifs(filename);
  return {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
}

size_t File::CalculateHash(const fs::path& filename) {
  const auto content = LoadContent(filename);
  auto res = content.size();
  for (const auto& c : content) {
    res ^= c + std::hash<fs::path>{}(filename) + 0x9e3779b9 + (res << 6) + (res >> 2);
  }
  return res;
}
