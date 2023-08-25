#include "filecomparator/FileComparator.h"

#include <fstream>

#include "dtl/dtl.hpp"

void FileComparator::Compare(const std::filesystem::path& p1,
                             const std::filesystem::path& p2) {
  std::ifstream f1(p1, std::ios_base::binary);
  std::ifstream f2(p2, std::ios_base::binary);
  const auto checkOpened = [](const char* path, const bool opened) {
    if (!opened) {
      throw std::runtime_error(fmt::format("File '{}' cannot be opened", path));
    }
  };

  checkOpened(p1.c_str(), f1.is_open());
  checkOpened(p2.c_str(), f2.is_open());

  using Element = std::string;
  using Sequence = std::vector<Element>;

  Element buf;
  Sequence ALines, BLines;

  while (getline(f1, buf)) {
    ALines.push_back(buf);
  }

  while (getline(f2, buf)) {
    BLines.push_back(buf);
  }

  dtl::Diff<Element> diff(ALines, BLines);
  diff.onHuge();
  diff.compose();

  if (diff.getEditDistance() > 0) {
    constexpr auto style = fmt::emphasis::bold;
    dtl::PrintOstreamFormatted(fmt::format("--- {}\n", p1.c_str()), std::cout, style);
    dtl::PrintOstreamFormatted(fmt::format("+++ {}\n", p2.c_str()), std::cout, style);
  }

  diff.composeUnifiedHunks();
  diff.printUnifiedFormat();
}
