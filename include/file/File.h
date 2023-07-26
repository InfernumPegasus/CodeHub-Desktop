#ifndef CODEHUB_FILE_H
#define CODEHUB_FILE_H

#include <filesystem>
#include "json/Json.h"

#include "FileStatus.h"

namespace fs = std::filesystem;

class File {
 public:
  File(fs::path filename, size_t hash, FileStatus status);

  explicit File(fs::path filename);

 public:
  [[nodiscard]] const fs::path& Name() const;

  [[nodiscard]] size_t Hash() const;

  [[nodiscard]] FileStatus Status() const;

 public:
  bool operator==(const File& rhs) const = default;

  bool operator<(const File& rhs) const;

 public:
  static std::vector<char> LoadContent(const fs::path& filename);

  static size_t CalculateHash(const fs::path& filename);

 private:
  const fs::path name_;

  const size_t hash_;
  const FileStatus status_;
};

namespace std {
template <>
struct hash<File> {
  std::size_t operator()(const File& file) const {
    return File::CalculateHash(file.Name());
  }
};
}  // namespace std

#endif  // CODEHUB_FILE_H
