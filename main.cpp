#include <boost/program_options.hpp>
#include <iostream>

#include "log/Logger.h"
#include "vcs/VersionControlSystem.h"

namespace po = boost::program_options;

// clang-format off
namespace {
constexpr auto HELP         = "help";
constexpr auto INIT         = "init";
constexpr auto STATUS       = "status";
constexpr auto DIFF         = "diff";
constexpr auto REPOSITORIES = "repositories";
constexpr auto LOG          = "log";
constexpr auto COMMIT       = "commit";
constexpr auto BRANCH       = "branch"; // TODO implement
constexpr auto BRANCHES     = "branches"; // TODO implement
constexpr auto CHECKOUT     = "checkout"; // TODO implement
constexpr auto RESTORE      = "restore";
constexpr auto PUSH         = "push";
}
// clang-format on

int main(int argc, char* argv[]) {
  VersionControlSystem vcs;
  vcs.Init();

  po::options_description description("Allowed options");
  // clang-format off
  description.add_options()
          (HELP, "produce help message")
          (INIT, po::value<std::string>(), "init new repository")
          (STATUS, "get repository status")
          (DIFF, po::value<std::string>(),
          "show file difference with last committed one")
          (REPOSITORIES, "show all repositories")
          (LOG, "show commits history")
          (COMMIT, po::value<std::string>(), "store changes")

          (BRANCH, po::value<std::string>(),"create new branch")
          (BRANCHES,"show all branches of this repository")

          (CHECKOUT, po::value<std::vector<std::string>>(), "switch to another branch")

          (RESTORE, po::value<size_t>(),
          "remove file from index or get it back to committed condition")
          (PUSH, "update external links and objects");
  // clang-format on

  try {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    if (vm.empty() || vm.contains(HELP)) {
      std::cout << description << std::endl;
    } else if (vm.contains(INIT)) {
      vcs.CreateRepository(vm[INIT].as<std::string>());
    } else if (vm.contains(STATUS)) {
      vcs.CheckStatus();
    } else if (vm.contains(DIFF)) {
      vcs.ShowFileDifference(vm[DIFF].as<std::string>());
    } else if (vm.contains(REPOSITORIES)) {
      vcs.ShowRepositories();
    } else if (vm.contains(LOG)) {
      vcs.CommitsLog();
    } else if (vm.contains(COMMIT)) {
      vcs.DoCommit(vm[COMMIT].as<std::string>());
    } else if (vm.contains(BRANCH)) {
      vcs.CreateBranch(vm[BRANCH].as<std::string>());
    } else if (vm.contains(BRANCHES)) {
      vcs.ShowBranches();
    } else if (vm.contains(PUSH)) {
      vcs.Push();
    } else if (vm.contains(RESTORE)) {
      vcs.RestoreFiles(vm[RESTORE].as<size_t>());
    }
  } catch (po::unknown_option& unknownOption) {
    logging::Log(LOG_EMERG, unknownOption.what());
    std::cout << description << std::endl;
    return EXIT_FAILURE;
  } catch (std::exception& exception) {
    logging::Log(LOG_EMERG, exception.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
