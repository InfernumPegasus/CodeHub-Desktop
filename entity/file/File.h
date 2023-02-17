#ifndef CODEHUB_FILE_H
#define CODEHUB_FILE_H

#include <filesystem>
#include <nlohmann/json.hpp>
#include "FileStatus.h"

class File {
public:
    File(std::string_view filename,
         size_t hash,
         FileStatus status) :
            name_(filename),
            hash_(hash),
            status_(status) {}

    explicit File(std::string_view filename) :
            name_(filename),
            hash_(CalculateHash(filename)),
            status_(FileStatus::Unknown) {}

    File(const File &rhs) = default;

    File &operator=(const File &rhs) = default;

    File(File &&rhs) noexcept = default;

    File &operator=(File &&rhs) noexcept = default;

public:
    [[nodiscard]] std::string Name() const;

    [[nodiscard]] size_t Hash() const;

    [[nodiscard]] FileStatus Status() const;

public:
    constexpr bool operator==(const File &rhs) const;

    bool operator<(const File &rhs) const;

//public:
//    [[nodiscard]] nlohmann::json ToJson() const;
//
//    static File FromJson(nlohmann::json json);

public:
    static size_t CalculateHash(std::string_view filename);

private:
    static std::vector<char> LoadContent(std::string_view filename);

private:
    std::string name_;

    size_t hash_;
    FileStatus status_;
};


#endif //CODEHUB_FILE_H
