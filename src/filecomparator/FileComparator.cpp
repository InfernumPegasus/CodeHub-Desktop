#include "filecomparator/FileComparator.h"

#include <fstream>
#include <functional>

#include "dtl/dtl.hpp"

void FileComparator::Compare(const std::filesystem::path& p1,
                             const std::filesystem::path& p2) {
  const auto checkOpened = [](const char* path, const bool opened) {
    if (!opened) {
      throw std::runtime_error(fmt::format("File '{}' cannot be opened", path));
    }
  };

  std::ifstream f1(p1, std::ios_base::binary);
  std::ifstream f2(p2, std::ios_base::binary);

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

void FileComparator::Merge(const std::filesystem::path& file1,
                           const std::filesystem::path& file2,
                           const std::filesystem::path& file3) {
  const auto readContent = [](std::string_view filename) {
    std::ifstream ifs(filename.data());
    if (!ifs) {
      throw std::runtime_error(fmt::format("File '{}' cannot be opened", filename));
    }
    return std::string{std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>()};
  };

  const auto newContent1 = readContent(file1.string());
  const auto newContent2 = readContent(file2.string());
  const auto newContent3 = readContent(file3.string());

  using Element = char;
  using Sequence = std::string;

  dtl::Diff3<Element, Sequence> diff3(newContent3, newContent1, newContent2);
  diff3.compose();
  if (!diff3.merge()) {
    throw std::runtime_error(
        fmt::format("Cannot merge files '{}', '{}' and '{}' because of conflict",
                    file1.c_str(), file2.c_str(), file3.c_str()));
  }

  std::cout << "Merging result:\n" << diff3.getMergedSequence() << std::endl;
}
