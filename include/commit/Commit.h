#ifndef CODEHUB_COMMIT_H
#define CODEHUB_COMMIT_H

#include <nlohmann/json.hpp>
#include <ostream>
#include <unordered_set>
#include <utility>

#include "file/File.h"

class Commit {
 public:
  Commit(const std::unordered_set<File>& files, std::string_view message);

  Commit(const std::unordered_set<std::string>& files, std::string_view message);

  Commit(std::unordered_set<File> files, std::string_view message, size_t checkSum);

 public:
  friend std::ostream& operator<<(std::ostream& os, const Commit& commit);

 public:
  [[nodiscard]] const std::unordered_set<File>& Files() const;

  [[nodiscard]] const std::string& Message() const;

  [[nodiscard]] size_t Checksum() const;

 public:
  [[nodiscard]] size_t CalculateCheckSum() const;

 public:
  bool operator==(const Commit& other) const = default;

 private:
  std::unordered_set<File> files_;
  std::string message_;
  size_t checkSum_;
};

#endif  // CODEHUB_COMMIT_H
