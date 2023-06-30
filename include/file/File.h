#ifndef CODEHUB_FILE_H
#define CODEHUB_FILE_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include "FileStatus.h"

class File {
public:
    File(std::string filename,
         size_t hash,
         FileStatus status);

    explicit File(std::string filename);

public:
    [[nodiscard]] const std::string &Name() const;

    [[nodiscard]] size_t Hash() const;

    [[nodiscard]] FileStatus Status() const;

public:
    constexpr bool operator==(const File &rhs) const = default;

    bool operator<(const File &rhs) const;

public:
    static std::vector<char> LoadContent(std::string_view filename);

    static size_t CalculateHash(std::string_view filename);

private:
    std::string name_;

    size_t hash_;
    FileStatus status_;
};

namespace std {
    template<>
    struct hash<File> {
        std::size_t operator()(const File &file) const {
            return File::CalculateHash(file.Name());
        }
    };
}


#endif //CODEHUB_FILE_H
