#ifndef CODEHUB_FILECOMPARATOR_H
#define CODEHUB_FILECOMPARATOR_H

#include <filesystem>
#include <map>
#include <string>
#include <utility>

class FileComparator {
 public:
  static void Compare(const std::filesystem::path& p1, const std::filesystem::path& p2);
};

#endif  // CODEHUB_FILECOMPARATOR_H
