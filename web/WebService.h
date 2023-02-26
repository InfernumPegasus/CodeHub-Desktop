#ifndef CODEHUB_WEBSERVICE_H
#define CODEHUB_WEBSERVICE_H

#include <nlohmann/json.hpp>
#include "cpr/cpr.h"
#include <iostream>

class WebService {
public:
    static constexpr std::string_view POST_LOGIN_TOKENS_URL =
            "http://localhost:8000/tokens/";

    static constexpr std::string_view POST_REGISTER_URL =
            "http://localhost:8000/users/";

    static constexpr std::string_view POST_REFRESH_URL =
            "http://localhost:8000/refresh/";

public:
    static void Login(std::string_view url, std::string_view userJson) {
        std::ifstream ifs(userJson.data());
        if (!ifs) {
            std::cerr << "Cannot open user file!";
            return;
        }
        try {
            nlohmann::json json = nlohmann::json::parse(ifs);
            std::string email = json["email"];
            std::string password = json["password"];

            cpr::Response response = cpr::Post(
                    cpr::Url{url},
                    cpr::Payload{
                            {"email",    email},
                            {"password", password}
                    }
            );
            std::cout << "text: " << response.text << "\ncookies:\n";
            for (const auto &item: response.cookies) {
                std::cout << item.GetName() << " " << item.GetValue() << "\n";
            }
        } catch (nlohmann::json::parse_error &parseError) {
            std::cerr << "Error: " << parseError.what();
        }
    }

    static void GetUser(int id) {
        cpr::Response response = cpr::Get(
                cpr::Url{POST_REGISTER_URL.data() + std::to_string(id)}
        );
        std::cout << response.text << "\n";
    }
};


#endif //CODEHUB_WEBSERVICE_H
