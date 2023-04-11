#include <iostream>
#include <boost/program_options.hpp>
#include "vcs/VersionControlSystem.h"
#include "web/WebService.h"

namespace po = boost::program_options;

using namespace std::string_literals;

int main(int argc, char *argv[]) {
//    VersionControlSystem versionControlSystem;
//    versionControlSystem.Init();
//
//    po::options_description description("Allowed options");
//    description.add_options()
//            ("help,h", "produce help message")
//            ("init", po::value<std::string>(), "init new repository")
//            ("status", "get repository status")
//            ("repositories", "show all repositories")
//            ("log", "show commits history")
//            ("commit", po::value<std::string>(), "store changes")
//            ("push", "update external links and objects");
//
//    try {
//        po::variables_map vm;
//        po::store(po::parse_command_line(argc, argv,
//                                         description), vm);
//
//        if (vm.empty() || vm.count("help")) {
//            std::cout << description << std::endl;
//        } else if (vm.count("init")) {
//            versionControlSystem.CreateRepository(
//                    vm["init"].as<std::string>(),
//                    true);
//        } else if (vm.count("status")) {
//            versionControlSystem.CheckStatus();
//        } else if (vm.count("repositories")) {
//            versionControlSystem.ShowRepositories();
//        } else if (vm.count("log")) {
//            VersionControlSystem::CommitsLog();
//        } else if (vm.count("commit")) {
//            VersionControlSystem::DoCommit(
//                    vm["commit"].as<std::string>()
//            );
//        } else if (vm.count("push")) {
//            WebService::PostLogin();
//            VersionControlSystem::Push();
//        }
//    } catch (po::unknown_option &unknownOption) {
//        std::cout << description << std::endl;
//    } catch (po::invalid_command_line_syntax &invalidCommandLineSyntax) {
//        std::cout << description << std::endl;
//    }

    auto response = WebService::PostLogin();
//    {
//        Repository repository("999QWERTY999",
//                              std::filesystem::current_path().string());
//        WebService::PostRepository(repository, true);
//    }
    {
//        auto repo = WebService::GetRepository("QWERTY");
//        std::cout << repo.Name() << " " << repo.Folder() << "\n";
//        for (const auto &commit: repo.Commits()) {
//            std::cout << commit << "\n";
//        }

        std::unordered_set<File> files{
//                {"/home/vladimir/GitHub/CodeHub/cmake-build-debug/CodeHub"s, 1, FileStatus::Created},
                {"/home/vladimir/GitHub/CodeHub/filemanager/Configs.h"s, 3, FileStatus::Deleted}
        };
        Commit commit1(files, "na bolote");
        Commit commit2(std::unordered_set<File>{}, "sueta");

        std::vector vec{commit1, commit2};
        Repository repository("lyagushki prigayut tuda suda",
                              std::filesystem::current_path().string(),
                              vec);
//        WebService::PostRepository(repository, true);
        std::cout << WebService::PostRepository(repository, true).text << "\n\n";
    }

    return 0;
}
