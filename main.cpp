#include <iostream>
#include <boost/program_options.hpp>
#include "vcs/VersionControlSystem.h"

namespace po = boost::program_options;

using namespace std::string_literals;

int main(int argc, char *argv[]) {
    VersionControlSystem vcs;
    vcs.Init();

    po::options_description description("Allowed options");
    description.add_options()
            ("help", "produce help message")
            ("init", po::value<std::string>(), "init new repository")
            ("status", "get repository status")
            ("diff", po::value<std::string>(),
             "shows file difference with last committed one")
            ("repositories", "show all repositories")
            ("log", "show commits history")
            ("commit", po::value<std::string>(), "store changes")
            ("restore", po::value<size_t>(),
             "remove file from index or get it back to committed condition")
            ("push", "update external links and objects");

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv,
                                         description), vm);
        if (vm.empty() || vm.count("help")) {
            std::cout << description << std::endl;
        } else if (vm.count("init")) {
            vcs.CreateRepository(vm["init"].as<std::string>());
        } else if (vm.count("status")) {
            vcs.CheckStatus();
        } else if (vm.count("diff")) {
            vcs.ShowFileDifference(vm["diff"].as<std::string>());
        } else if (vm.count("repositories")) {
            vcs.ShowRepositories();
        } else if (vm.count("log")) {
            vcs.CommitsLog();
        } else if (vm.count("commit")) {
            vcs.DoCommit(vm["commit"].as<std::string>());
        } else if (vm.count("push")) {
            vcs.Push();
        } else if (vm.count("restore")) {
            vcs.RestoreFiles(vm["restore"].as<size_t>());
        }
    } catch (po::unknown_option &unknownOption) {
        std::cout << description << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
