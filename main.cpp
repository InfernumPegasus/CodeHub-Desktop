#include "entity/repository/Repository.h"
#include "entity/VersionControlSystem.h"
#include <boost/program_options.hpp>

using namespace std::string_literals;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    VersionControlSystem versionControlSystem;
    versionControlSystem.Init();

//    versionControlSystem.CreateRepository("LOL KEK CHEBUREK");
    versionControlSystem.CreateRepository("nu zdarova /*/");

    for (const auto &item: versionControlSystem.NameAndFolderMap()) {
        std::cout << item.first << " | " << item.second << "\n";
    }

//    repository.Init();
//    repository.DoCommit("QWERTY");

//    std::cout << std::boolalpha <<
//              Validator::IsValidRepositoryName("qwem__32") << "\n";

//    WebService::Login(WebService::POST_LOGIN_TOKENS_URL,
//                      "../.config/.user.json");
//    WebService::GetUser(1);

    return 0;
}
