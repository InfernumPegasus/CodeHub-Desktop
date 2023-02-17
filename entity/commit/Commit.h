#ifndef CODEHUB_COMMIT_H
#define CODEHUB_COMMIT_H


#include <set>
#include <nlohmann/json.hpp>
#include <utility>
#include "../file/File.h"

class Commit {
public:
    Commit(const std::set<File> &files,
           std::string message) :
            message_(std::move(message)) {
        for (const auto &file: files) {
            files_.emplace(file);
        }
        checkSum_ = CalculateCheckSum();
    }

    Commit(const std::set<std::string> &files,
           std::string message) :
            message_(std::move(message)) {
        for (const auto &file: files) {
            files_.emplace(file);
        }
        checkSum_ = CalculateCheckSum();
    }

    Commit(std::set<File> files,
           std::string message,
           size_t checkSum) :
            files_(std::move(files)),
            message_(std::move(message)),
            checkSum_(checkSum) {}

    Commit(const Commit &rhs) = default;

    Commit &operator=(const Commit &rhs) = default;

    Commit(Commit &&rhs) noexcept = default;

    Commit &operator=(Commit &&rhs) noexcept = default;

public:
    [[nodiscard]] std::set<File> Files() const;

    [[nodiscard]] constexpr std::string Message() const;

    [[nodiscard]] constexpr size_t Checksum() const;

public:
    [[nodiscard]] nlohmann::json ToJson() const;

    static Commit FromJson(nlohmann::json json);

public:
    size_t CalculateCheckSum() {
        size_t checkSum = 0;
        auto size = files_.size();
        for (const auto &file: files_) {
            checkSum += file.Hash() + (size << 2) + (size >> 5);
        }
        checkSum += std::hash<std::string>{}(message_);
        return checkSum;
    }

private:
    std::set<File> files_;
    std::string message_;
    size_t checkSum_;
};


#endif //CODEHUB_COMMIT_H
