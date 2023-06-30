#ifndef CODEHUB_FILECOMPARATOR_H
#define CODEHUB_FILECOMPARATOR_H

#include <filesystem>
#include <map>
#include <ostream>
#include <string>
#include <utility>

class FileComparator {
  using Lines = std::pair<std::string, std::string>;
  using LineNumber = unsigned int;

  using FileDifference = std::map<LineNumber, Lines>;

 public:
  static FileDifference Compare(const std::filesystem::path& p1,
                                const std::filesystem::path& p2);
};

#endif  // CODEHUB_FILECOMPARATOR_H
