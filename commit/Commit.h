#ifndef CODEHUB_COMMIT_H
#define CODEHUB_COMMIT_H

#include "../file/File.h"
#include <set>
#include <nlohmann/json.hpp>
#include <utility>

class Commit {
public:
    Commit(const std::set<File> &files,
           std::string message);

    Commit(const std::set<std::string> &files,
           std::string message);

    Commit(std::set<File> files,
           std::string message,
           size_t checkSum);

    Commit(const Commit &rhs) = default;

    Commit &operator=(const Commit &rhs) = default;

    Commit(Commit &&rhs) noexcept = default;

    Commit &operator=(Commit &&rhs) noexcept = default;

public:
    [[nodiscard]] std::set<File> Files() const;

    [[nodiscard]] std::string Message() const;

    [[nodiscard]] size_t Checksum() const;

public:
    [[nodiscard]] size_t CalculateCheckSum() const;

private:
    std::set<File> files_;
    std::string message_;
    size_t checkSum_;
};


#endif //CODEHUB_COMMIT_H
