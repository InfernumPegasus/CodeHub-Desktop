#ifndef CODEHUB_FILECOMPARATOR_H
#define CODEHUB_FILECOMPARATOR_H

#include <filesystem>
#include <map>
#include <string>
#include <utility>

class FileComparator {
 public:
  static void Compare(const std::filesystem::path& p1, const std::filesystem::path& p2);

  static void Merge(const std::filesystem::path& file1,
                    const std::filesystem::path& file2,
                    const std::filesystem::path& file3);
};

#endif  // CODEHUB_FILECOMPARATOR_H
