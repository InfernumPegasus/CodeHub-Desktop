#include "repository/Repository.h"
#include "vcs/VersionControlSystem.h"
#include "validation/Validator.h"
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

using namespace std::string_literals;
namespace po = boost::program_options;

VersionControlSystem versionControlSystem;

void InitOptionHandle(const std::string &arg) {
    using boost::char_separator;

    char_separator<char> separator(" \t\n\r");

    boost::tokenizer<char_separator<char>> tokenizer(arg, separator);
    std::vector<std::string> args;
    std::copy(tokenizer.begin(), tokenizer.end(),
              std::back_inserter(args));

    if (!std::filesystem::exists(args[1]) ||
        !Validator::IsValidRepositoryName(args[0])) {
        std::cout << "Cannot create repository: invalid name or path!\n";
    } else if (versionControlSystem.Contains(args[0])) {
        std::cout << "Repository '" << args[0] << "' already exists.\n";
    } else {
        Repository repository(args[0], args[1]);
        versionControlSystem.CreateRepository(args[0],
                                              args[1],
                                              true);
    }
}


int main(int argc, char *argv[]) {
    versionControlSystem.Init();

    po::options_description description("Allowed options");
    description.add_options()
            ("help,h", "produce help message")
            ("init", po::value<std::string>(), "init new repository")
            ("status", po::value<std::string>(), "get repository status")
            ("add", po::value<std::vector<std::string >>(), "add file to index")
            ("restore", "restore files in work directory")
            ("rm", po::value<std::vector<std::string>>(), "remove files from index and directory")
            ("log", "show commits history")
            ("commit", "store changes")
            ("push", "update external links and objects");

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, description), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << description << std::endl;
        } else if (vm.count("init")) {
            InitOptionHandle(vm["init"].as<std::string>());
        } else if (vm.count("status")) {
            versionControlSystem.CheckStatus(vm["status"].as<std::string>());
        }
    } catch (po::unknown_option &unknownOption) {
        std::cout << unknownOption.what() << std::endl;
    }

//    std::string str = "hello world bye world haha";
//    std::cout << get_last_word(str);

//    versionControlSystem.CreateRepository("LOL KEK CHEBUREK");
//    versionControlSystem.CreateRepository("nu zdarova /*/");

//    repository.InitManagers();
//    repository.DoCommit("QWERTY");

//    WebService::Login(WebService::POST_LOGIN_TOKENS_URL,
//                      "../.config/.user.json");
//    WebService::GetUser(1);

    return 0;
}
