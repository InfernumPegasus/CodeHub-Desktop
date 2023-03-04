#include "repository/Repository.h"
#include "vcs/VersionControlSystem.h"
#include "validation/Validator.h"
#include "web/WebService.h"
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

using namespace std::string_view_literals;
namespace po = boost::program_options;


//std::vector<std::string> SplitByWhitespace(std::string_view string) {
//    boost::char_separator<char> separator(" \t\n\r");
//
//    boost::tokenizer<boost::char_separator<char>> tokenizer(string, separator);
//    std::vector<std::string> args;
//    std::copy(tokenizer.begin(), tokenizer.end(),
//              std::back_inserter(args));
//
//    return args;
//}

VersionControlSystem versionControlSystem;

int main(int argc, char *argv[]) {
//    versionControlSystem.Init();
//    po::options_description description("Allowed options");
//    description.add_options()
//            ("help,h", "produce help message")
//            ("init", po::value<std::string>(), "init new repository")
//            ("status", "get repository status")
//            ("repositories", "show all repositories")
//            ("add", po::value<std::string>(), "add file to index")
//            ("log", "show commits history")
//            ("commit", po::value<std::string>(), "store changes")
//            ("push", "update external links and objects");
//
//    try {
//        po::variables_map vm;
//        po::store(po::parse_command_line(argc, argv, description), vm);
//        po::notify(vm);
//
//        if (vm.count("help")) {
//            std::cout << description << std::endl;
//        } else if (vm.count("init")) {
//            versionControlSystem.CreateRepository(
//                    vm["init"].as<std::string>(),
//                    true);
//        } else if (vm.count("status")) {
//            versionControlSystem.CheckStatus();
//        } else if (vm.count("repositories")) {
//            versionControlSystem.ShowRepositories();
//        } else if (vm.count("add")) {
//            std::cout << "Will be added later, maybe =)\n";
////            VersionControlSystem::AddFiles(
////                    SplitByWhitespace(vm["add"].as<std::string>())
////            );
//        } else if (vm.count("log")) {
//            VersionControlSystem::CommitsLog();
//        } else if (vm.count("commit")) {
//            VersionControlSystem::DoCommit(
//                    vm["commit"].as<std::string>()
//            );
//        } else if (vm.count("push")) {
//        }
//    } catch (po::unknown_option &unknownOption) {
//        std::cout << unknownOption.what() << std::endl;
//    }

    auto login = WebService::PostLogin(".config/user.json"sv);
    auto tokens = WebService::PostTokens(".config/user.json"sv);
    auto r = WebService::GetCommits();
    std::cout << login.text << "\n\n";
    std::cout << tokens.text << "\n\n";
    std::cout << r.text << "\n\n";
//    WebService::GetUser(0);

    return 0;
}
