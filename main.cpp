#include "repository/Repository.h"
#include "vcs/VersionControlSystem.h"
#include "validation/Validator.h"
#include "web/WebService.h"
#include <boost/program_options.hpp>

using namespace std::string_view_literals;
using namespace std::string_literals;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    VersionControlSystem versionControlSystem;
    versionControlSystem.Init();

    po::options_description description("Allowed options");
    description.add_options()
            ("help,h", "produce help message")
            ("init", po::value<std::string>(), "init new repository")
            ("status", "get repository status")
            ("repositories", "show all repositories")
            ("log", "show commits history")
            ("commit", po::value<std::string>(), "store changes")
            ("push", "update external links and objects");

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv,
                                         description), vm);

        if (vm.empty() || vm.count("help")) {
            std::cout << description << std::endl;
        } else if (vm.count("init")) {
            versionControlSystem.CreateRepository(
                    vm["init"].as<std::string>(),
                    true);
        } else if (vm.count("status")) {
            versionControlSystem.CheckStatus();
        } else if (vm.count("repositories")) {
            versionControlSystem.ShowRepositories();
        } else if (vm.count("log")) {
            VersionControlSystem::CommitsLog();
        } else if (vm.count("commit")) {
            VersionControlSystem::DoCommit(
                    vm["commit"].as<std::string>()
            );
        } else if (vm.count("push")) {
            WebService::PostLogin(".config/user.json"sv);
            VersionControlSystem::Push();
        }
    } catch (po::unknown_option &unknownOption) {
        std::cout << unknownOption.what() << std::endl;
    }

//    auto login = WebService::PostLogin(".config/user.json"sv);
//    File file1("build.ninja");
//    File file2(".ninja_log");
//    Commit commit(std::set{file1, file2}, "baza");
//
//    VersionControlSystem::Push();

    return 0;
}
