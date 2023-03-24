#ifndef CODEHUB_WEBSERVICE_H
#define CODEHUB_WEBSERVICE_H

#include <nlohmann/json.hpp>
#include "cpr/cpr.h"
#include "Endpoints.h"
#include "../repository/Repository.h"

class WebService {
public:
    static cpr::Cookies GetCookiesFromFile();

    static void SaveCookiesInFile(const cpr::Cookies& cookies);

public:
    static cpr::Response PostLogin();

    static size_t GetCurrentUser();

public:
    static Commit GetCommit(size_t id);

    static std::vector<Commit> GetCommits(const std::vector<size_t> &ids);

public:
    static void PostFiles(const std::unordered_set<File> &files);

    static cpr::Response PostFile(const File &file);

public:
    static cpr::Response GetRefresh();

public:
    static void GetRepositories();

    static Repository GetRepository(const std::string &repoName);

    static cpr::Response PostRepository(const Repository &repository,
                                        bool isPrivate);

    static cpr::Response PatchRepository(std::string_view repoName,
                                         const Repository &repository,
                                         bool isPrivate);

public:
    static cpr::Files AttachFiles(const std::unordered_set<std::string> &filesVector);
};


#endif //CODEHUB_WEBSERVICE_H
