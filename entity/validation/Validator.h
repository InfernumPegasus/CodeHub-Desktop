#ifndef CODEHUB_VALIDATOR_H
#define CODEHUB_VALIDATOR_H

#include <string_view>

class Validator {
public:
    static bool IsValidEmail(std::string_view email);

    static bool IsValidRepositoryName(std::string_view repositoryName);

public:
    static void Trim(std::string &string);

    static void ReplaceWith(std::string &string,
                            std::string_view what,
                            std::string_view to);
};


#endif //CODEHUB_VALIDATOR_H
