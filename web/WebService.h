#ifndef CODEHUB_WEBSERVICE_H
#define CODEHUB_WEBSERVICE_H

#include <nlohmann/json.hpp>
#include "cpr/cpr.h"
#include "Endpoints.h"

class WebService {
private:
    static cpr::Response PostLogin(nlohmann::json userJson);

    static cpr::Response PostTokens(nlohmann::json userJson);

public:
    static cpr::Response PostLogin(std::string_view pathToJson);

    static cpr::Response PostTokens(std::string_view pathToJson);


    static void GetUser(int id);

public:
    static cpr::Response GetCommits();

};


#endif //CODEHUB_WEBSERVICE_H
