#include "filemanager/RestoreFileManager.h"

namespace fs = std::filesystem;

bool RestoreFileManager::CreateFolder(const fs::path& folder) {
  return fs::create_directories(folder) && fs::exists(folder);
}

void RestoreFileManager::CopyFiles(const types::FilesSet& files, const fs::path& from,
                                   const fs::path& to) {
  for (const auto& file : files) {
    if (file.Status() == FileStatus::Deleted) continue;

    const auto targetParentPath = to / fs::relative(file.Name(), from).parent_path();

    fs::create_directories(targetParentPath);
    fs::copy(file.Name(), targetParentPath, fs::copy_options::overwrite_existing);
  }
}

void RestoreFileManager::CopyFolderRecursive(const fs::path& from, const fs::path& to) {
  fs::copy(from, to, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
}
