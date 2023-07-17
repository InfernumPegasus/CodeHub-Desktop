#include "filemanager/RestoreFileManager.h"

#include "log/Logger.h"

namespace fs = std::filesystem;

bool RestoreFileManager::CreateFolder(const fs::path& folder) {
  const auto res = fs::create_directories(folder) && fs::exists(folder);
  logging::Log(LOG_WARNING,
               fmt::format("Folder '{}' {} created", folder.c_str(), res ? "" : "NOT"));
  return res;
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
