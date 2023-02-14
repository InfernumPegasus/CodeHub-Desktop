#include "entity/repository/Repository.h"
#include <cpr/cpr.h>

using namespace std::string_literals;

int main() {
    Repository repository("Test-Rep",
                          "/home/vladimir/GitHub/CodeHub/");
    repository.Init();
    repository.DoCommit("QWERTY");

//    nlohmann::json j;
//    j["name"] = "Vladimir";
//    j["age"] = 19;
//
//    cpr::Response r = cpr::Get(
//            cpr::Url{"http://localhost:8000/search/hhggtyj"}
////            cpr::Parameters{
////                    {"name", j["name"]},
////                    {"age", to_string(j["age"])}
////            }
//    );
//
//    std::cout << r.status_code << std::endl;
//    std::cout << r.header["content-type"] << std::endl;
//    std::cout << r.text << std::endl;

    return 0;
}
