#ifndef CODEHUB_ENDPOINTS_H
#define CODEHUB_ENDPOINTS_H

#include <string_view>

/* USERS endpoints */

const std::string BASE_USERS_URL =
        "http://localhost:8000/users/";

/* COMMITS endpoints */

const std::string BASE_COMMITS_URL =
        "http://localhost:8000/commits/";

/* TOKENS endpoints*/

const std::string BASE_TOKENS_URL =
        "http://localhost:8000/tokens/";

/* FILES endpoints */

const std::string BASE_FILES_URL =
        "http://localhost:8000/files/";

/* REFRESH endpoints */
const std::string BASE_REFRESH_URL =
        "http://localhost:8000/refresh/";

/* REPOSITORIES endpoints */
const std::string BASE_REPOSITORIES_URL =
        "http://localhost:8000/repositories/";


#endif //CODEHUB_ENDPOINTS_H
