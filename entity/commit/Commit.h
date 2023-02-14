#ifndef CODEHUB_COMMIT_H
#define CODEHUB_COMMIT_H


#include <set>
#include <nlohmann/json.hpp>
#include "../file/File.h"

// TODO добавить хэш-сумму и уникальный ID коммита
class Commit {
public:
    Commit(const std::set<File> &files, std::string message) :
            message_(std::move(message)) {
        for (const auto &file: files) {
            files_.emplace(file);
        }
    }

    Commit(const std::set<std::string> &files, std::string message) :
            message_(std::move(message)) {
        for (const auto &file: files) {
            files_.emplace(file);
        }
    }

    Commit(const Commit &rhs) = default;

    Commit &operator=(const Commit &rhs) = default;

    Commit(Commit &&rhs) noexcept = default;

    Commit &operator=(Commit &&rhs) noexcept = default;

public:
    [[nodiscard]] std::set<File> Files() const;

    [[nodiscard]] constexpr std::string Message() const;

public:
    [[nodiscard]] nlohmann::json ToJson() const;

    static Commit FromJson(nlohmann::json json);

private:
    std::set<File> files_;
    std::string message_;
};


#endif //CODEHUB_COMMIT_H
