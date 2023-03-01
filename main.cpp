#include "repository/Repository.h"
#include "vcs/VersionControlSystem.h"
#include "validation/Validator.h"
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

using namespace std::string_literals;
namespace po = boost::program_options;

VersionControlSystem versionControlSystem;

std::vector<std::string> SplitByWhitespace(std::string_view string) {
    boost::char_separator<char> separator(" \t\n\r");

    boost::tokenizer<boost::char_separator<char>> tokenizer(string, separator);
    std::vector<std::string> args;
    std::copy(tokenizer.begin(), tokenizer.end(),
              std::back_inserter(args));

    return args;
}

int main(int argc, char *argv[]) {
//    Repository r("TEST_REPO", std::filesystem::current_path().string());
//    r.InitManagers();
//    r.DoCommit("QWERTY");
//    for (const auto &[name, hash]: repository.Map()) {
//        std::cout << name << " : " << hash << std::endl;
//    }
//    std::cout << std::endl;
//
//    for (const auto &item: repository.Commits()) {
//        std::cout << item.Message() << std::endl;
//    }
//
//    repository.DoCommit("test msg");
//    for (const auto &item: repository.Commits()) {
//        std::cout << item.Message() << ":" << std::endl;
//    }

    versionControlSystem.Init();
//    versionControlSystem.CreateRepository("TEST REPO", true);
    versionControlSystem.ShowRepositories();
    for (const auto &[name, folder]:
            versionControlSystem.NameAndFolderMap()) {
        Repository repository(name, folder);
        repository.InitManagers();

        for (const auto &[repoName, hash]: repository.Map()) {
            std::cout << repoName << " : " << hash << std::endl;
        }
        std::cout << std::endl;

        for (const auto &item: repository.Commits()) {
            std::cout << item.Message() << std::endl;
        }
    }

//    po::options_description description("Allowed options");
//    description.add_options()
//            //есть
//            ("help,h", "produce help message")
//            //есть но надо тестить
//            ("init", po::value<std::string>(), "init new repository")
//            //есть но надо тестить
//            ("status", "get repository status")
//            ("repositories", "show all repositories")
//            ("add", po::value<std::string>(), "add file to index")
////            ("restore", "restore files in work directory")
//            ("rm", po::value<std::vector<std::string>>(), "remove files from index and directory")
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
//            // TODO а нужно ли оно вообще? (не особо но пусть будет)
//            VersionControlSystem::AddFiles(
//                    SplitByWhitespace(vm["add"].as<std::string>())
//            );
//        } else if (vm.count("rm")) {
//            // TODO допилить
//            std::cout << "TODO\n";
//        } else if (vm.count("commit")) {
//            VersionControlSystem::DoCommit(
//                    vm["commit"].as<std::string>()
//            );
//        }
//    } catch (po::unknown_option &unknownOption) {
//        std::cout << unknownOption.what() << std::endl;
//    }

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
