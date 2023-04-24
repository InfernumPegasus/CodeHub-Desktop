#include <iostream>
#include <boost/program_options.hpp>
#include "vcs/VersionControlSystem.h"
#include "web/WebService.h"

namespace po = boost::program_options;

using namespace std::string_literals;

int main(int argc, char *argv[]) {
    VersionControlSystem versionControlSystem;
    versionControlSystem.Init();

    po::options_description description("Allowed options");
    description.add_options()
            ("help", "produce help message")
            ("init", po::value<std::string>(), "init new repository")
            ("status", "get repository status")
            ("repositories", "show all repositories")
            ("log", "show commits history")
            ("commit", po::value<std::string>(), "store changes")
            ("push", "update external links and objects")
            ("restore", po::value<int32_t>(),
             "remove file from index or get it back to committed condition");

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
            WebService::PostLogin();
            VersionControlSystem::Push();
        } else if (vm.count("restore")) {
            VersionControlSystem::RestoreFiles(
                    vm["restore"].as<int32_t>()
            );
        }
    } catch (po::unknown_option &unknownOption) {
        std::cout << description << std::endl;
    } catch (po::invalid_command_line_syntax &invalidCommandLineSyntax) {
        std::cout << description << std::endl;
    }

    return 0;
}
