#ifndef CODEHUB_WEBSERVICE_H
#define CODEHUB_WEBSERVICE_H

#include <optional>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "config/Endpoints.h"
#include "repository/Repository.h"

class WebService {
public:
    static bool NoErrorsInResponse(long statusCode);

public:
    static cpr::Response PostLogin(std::string_view email,
                                   std::string_view password);

    static int GetCurrentUser();

public:
    static cpr::Response PostCommit(const Commit &commit);

    static std::vector<int> PostCommits(const std::vector<Commit> &commits);

public:
    static std::vector<int> PostFiles(const std::unordered_set<File> &files);

    static cpr::Response PostFile(const File &file);

public:
    static std::optional<Repository> GetRepository(const std::string &repoName);

    static nlohmann::json GetRepositoryJson(const std::string &repoName);

    static cpr::Response PostRepository(const Repository &repository);

    static cpr::Response PatchRepository(std::string_view repoName,
                                         const Repository &repository,
                                         bool isPrivate,
                                         const std::vector<Commit> &commits);
};

#endif //CODEHUB_WEBSERVICE_H
