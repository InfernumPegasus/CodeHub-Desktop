#ifndef CODEHUB_ENDPOINTS_H
#define CODEHUB_ENDPOINTS_H

#include <string_view>

/* USERS endpoints */

constexpr std::string_view BASE_USERS_URL =
        "http://localhost:8000/users/";

/* COMMITS endpoints */

constexpr std::string_view BASE_COMMITS_URL =
        "http://localhost:8000/commits/";

/* TOKENS endpoints*/

constexpr std::string_view BASE_TOKENS_URL =
        "http://localhost:8000/tokens/";

/* FILES endpoints */

constexpr std::string_view BASE_FILES_URL =
        "http://localhost:8000/files/";

/* REFRESH endpoints */
constexpr std::string_view BASE_REFRESH_URL =
        "http://localhost:8000/refresh/";

/* REPOSITORIES endpoints */
constexpr std::string_view BASE_REPOSITORIES_URL =
        "http://localhost:8000/repositories/";


#endif //CODEHUB_ENDPOINTS_H
