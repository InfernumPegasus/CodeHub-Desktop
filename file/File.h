#ifndef CODEHUB_FILE_H
#define CODEHUB_FILE_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include "FileStatus.h"

class File {
public:
    File(std::string filename,
         int32_t hash,
         FileStatus status);

    explicit File(std::string filename);

    File(const File &rhs) = default;

    File &operator=(const File &rhs) = default;

    File(File &&rhs) noexcept = default;

    File &operator=(File &&rhs) noexcept = default;

public:
    [[nodiscard]] std::string Name() const;

    [[nodiscard]] int32_t Hash() const;

    [[nodiscard]] FileStatus Status() const;

public:
    constexpr bool operator==(const File &rhs) const = default;

    bool operator<(const File &rhs) const;

public:
    static int32_t CalculateHash(std::string_view filename);

private:
    static std::vector<char> LoadContent(std::string_view filename);

private:
    std::string name_;

    int32_t hash_;
    FileStatus status_;
};

namespace std {
    template <>
    struct hash<File> {
        std::size_t operator()(const File& file) const {
            return File::CalculateHash(file.Name());
        }
    };
}


#endif //CODEHUB_FILE_H
