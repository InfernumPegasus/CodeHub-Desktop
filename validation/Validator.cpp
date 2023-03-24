#include <regex>
#include <boost/algorithm/string.hpp>
#include "Validator.h"

bool Validator::IsValidEmail(std::string_view email) {
    static const std::regex mailRegex
            (R"(/^[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?$/)");

    return std::regex_match(
            std::cbegin(email),
            std::cend(email),
            mailRegex
    );
}

bool Validator::IsValidRepositoryName(std::string_view repositoryName) {
    static const std::regex repositoryNameRegex
            (R"(^[A-Za-z][A-Za-z0-9_]{3,50}$)");

    return std::regex_match(
            std::cbegin(repositoryName),
            std::cend(repositoryName),
            repositoryNameRegex
    );
}

void Validator::Trim(std::string &string) {
    boost::algorithm::trim(string);
}

void Validator::Replace(std::string &string,
                        std::string_view what,
                        std::string_view to) {
    boost::replace_all(string, what, to);
}

void Validator::ValidateRepositoryName(std::string &name) {
    Trim(name);
    Replace(name, " ", "_");
}

