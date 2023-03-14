#ifndef CODEHUB_WEBSERVICE_H
#define CODEHUB_WEBSERVICE_H

#include <nlohmann/json.hpp>
#include "cpr/cpr.h"
#include "Endpoints.h"
#include "../commit/Commit.h"

class WebService {
public:
    static cpr::Response PostLogin(std::string_view pathToJson);

    static cpr::Response PostLogin(const nlohmann::json& userJson);

public:
    static std::vector<int> PostFiles(const std::set<File> &files);

    static cpr::Response PostFile(const File &file);

public:
    static cpr::Response PostCommit(const Commit &commit);

    static std::vector<int> PostCommits(const std::vector<Commit> &commits);

public:
    static void GetUser(int id);

public:
    static Commit GetCommit(size_t hash);

    static std::vector<Commit> GetCommits(const std::vector<int> &ids);

public:
    static std::set<File> GetFiles(const std::vector<int> &ids);

    static File GetFile(int id);
};


#endif //CODEHUB_WEBSERVICE_H
