#ifndef CODEHUB_FILECOMPARATOR_H
#define CODEHUB_FILECOMPARATOR_H

#include <utility>
#include <string>
#include <map>
#include <filesystem>
#include <ostream>

class FileComparator {
    using Lines = std::pair<std::string, std::string>;
    using LineNumber = unsigned int;

    using FileDifference = std::map<LineNumber, Lines>;

public:
    static FileDifference Compare(const std::filesystem::path &p1,
                                  const std::filesystem::path &p2);
};

#endif //CODEHUB_FILECOMPARATOR_H
