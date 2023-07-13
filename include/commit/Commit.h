#ifndef CODEHUB_COMMIT_H
#define CODEHUB_COMMIT_H

#include <nlohmann/json.hpp>
#include <ostream>
#include <unordered_set>
#include <utility>

#include "config/Types.h"
#include "file/File.h"

class Commit {
 public:
  Commit(types::FilesSet files, std::string message);

  Commit(const std::unordered_set<std::string>& files, std::string message);

  Commit(types::FilesSet files, std::string message, size_t checkSum);

 public:
  friend std::ostream& operator<<(std::ostream& os, const Commit& commit);

 public:
  [[nodiscard]] const types::FilesSet& Files() const;

  [[nodiscard]] const std::string& Message() const;

  [[nodiscard]] size_t Checksum() const;

 public:
  [[nodiscard]] size_t CalculateCheckSum() const;

 public:
  bool operator==(const Commit& other) const = default;

 private:
  types::FilesSet files_;
  std::string message_;
  size_t checkSum_;
};

namespace types {
using Commits = std::list<Commit>;
}

#endif  // CODEHUB_COMMIT_H
