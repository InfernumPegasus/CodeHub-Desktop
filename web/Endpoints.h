#ifndef CODEHUB_ENDPOINTS_H
#define CODEHUB_ENDPOINTS_H

#include <string_view>

/* USERS endpoints */

static constexpr std::string_view BASE_USERS_URL =
        "http://localhost:8000/users/";

/* COMMITS endpoints */

static constexpr std::string_view BASE_COMMITS_URL =
        "http://localhost:8000/commits/";

/* TOKENS endpoints*/

static constexpr std::string_view BASE_TOKENS_URL =
        "http://localhost:8000/tokens/";

/* FILES endpoints */

static constexpr std::string_view BASE_FILES_URL =
        "http://localhost:8000/files/";

/* REFRESH endpoints */
static constexpr std::string_view BASE_REFRESH_URL =
        "http://localhost:8000/refresh/";

/* REPOSITORIES endpoints */
static constexpr std::string_view BASE_REPOSITORIES_URL =
        "http://localhost:8000/repositories/";


#endif //CODEHUB_ENDPOINTS_H
