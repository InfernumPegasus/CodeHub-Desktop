#include "filemanager/RestoreFileManager.h"

#include <iostream>

namespace fs = std::filesystem;

bool RestoreFileManager::CreateRecoveryFolder(const fs::path& folder) {
  return !fs::exists(folder) && fs::create_directory(folder);
}

void RestoreFileManager::CopyFiles(const std::unordered_set<File>& files,
                                   const fs::path& from, const fs::path& to) {
  try {
    for (const auto& file : files) {
      if (file.Status() == FileStatus::Deleted) continue;
      const auto targetParentPath = to / fs::relative(file.Name(), from).parent_path();
      fs::create_directories(targetParentPath);

      fs::copy(file.Name(), targetParentPath, fs::copy_options::overwrite_existing);
    }
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
  }
}

void RestoreFileManager::CopyRecursive(const fs::path& from, const fs::path& to) {
  try {
    fs::copy(from, to,
             fs::copy_options::overwrite_existing | fs::copy_options::recursive);
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
  }
}
