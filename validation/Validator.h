#ifndef CODEHUB_VALIDATOR_H
#define CODEHUB_VALIDATOR_H

#include <string_view>

class Validator {
public:
    static bool IsValidRepositoryName(std::string_view repositoryName);

public:
    static void Trim(std::string &string);

    static void Replace(std::string &string,
                        std::string_view what,
                        std::string_view to);

    static void ValidateRepositoryName(std::string &name);
};


#endif //CODEHUB_VALIDATOR_H
