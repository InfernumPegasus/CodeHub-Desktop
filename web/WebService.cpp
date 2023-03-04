#include <iostream>
#include "WebService.h"

cpr::Response WebService::PostLogin(std::string_view pathToJson) {
    std::ifstream ifs(pathToJson.data());
    nlohmann::json j = nlohmann::json::parse(ifs);
    return PostLogin(j);
}

cpr::Response WebService::PostLogin(nlohmann::json userJson) {
    return cpr::Post(
            cpr::Url{BASE_TOKENS_URL},
            cpr::Payload{
                    {"email",    userJson["email"]},
                    {"password", userJson["password"]}
            }
    );
}

cpr::Response WebService::PostTokens(std::string_view pathToJson) {
    std::ifstream ifs(pathToJson.data());
    nlohmann::json j = nlohmann::json::parse(ifs);
    return PostTokens(j);
}

cpr::Response WebService::PostTokens(nlohmann::json userJson) {
    return cpr::Post(
            cpr::Url{BASE_TOKENS_URL},
            cpr::Payload{
                    {"email",    userJson["email"]},
                    {"password", userJson["password"]}
            }
    );
}

void WebService::GetUser(int id) {
    cpr::Response response = cpr::Get(
            cpr::Url{BASE_USERS_URL.data() + std::to_string(id)}
    );
    std::cout << response.text << "\n";
}

cpr::Response WebService::GetCommits() {
    return cpr::Get(
            cpr::Url{BASE_COMMITS_URL}
    );
}
