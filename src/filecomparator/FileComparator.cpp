#include "filecomparator/FileComparator.h"

#include <fstream>

FileComparator::FileDifference FileComparator::Compare(const std::filesystem::path& p1,
                                                       const std::filesystem::path& p2) {
  FileDifference difference;
  if (p1 == p2) {
    return difference;
  }

  std::ifstream f1(p1, std::ios_base::binary);
  std::ifstream f2(p2, std::ios_base::binary);
  if (!f1 || !f2) {
    throw std::runtime_error("Could not open files");
  }

  LineNumber current{0};
  while (true) {
    ++current;

    if (f1.eof() && f2.eof()) {
      break;
    }

    std::string line1, line2;
    getline(f1, line1);
    getline(f2, line2);
    if (line1 != line2) {
      difference[current] = {line1, line2};
    }
  }

  return difference;
}