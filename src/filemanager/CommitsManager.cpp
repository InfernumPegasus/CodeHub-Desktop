#include "filemanager/CommitsManager.h"

#include <fstream>
#include <utility>

#include "serializer/JsonSerializer.h"
#include "utils/ConfigFiles.h"

CommitsManager::CommitsManager(const fs::path& commitsFolder, types::Commits* commits)
    : commitsFile_(std::move(commitsFolder / COMMITS_FILE)), commitsRef_(*commits) {}

bool CommitsManager::Create() {
  std::ofstream file(commitsFile_);
  logging::Log(LOG_WARNING,
               fmt::format("Commits file {} created in folder '{}'",
                           file.is_open() ? "" : "NOT", commitsFile_.c_str()));
  return file.is_open();
}

bool CommitsManager::Read() {
  if (!std::filesystem::exists(commitsFile_)) {
    return false;
  }

  if (std::filesystem::is_empty(commitsFile_)) {
    return true;
  }

  std::ifstream ifs(commitsFile_);

  nlohmann::json j = nlohmann::json::parse(ifs);
  const auto readCommits = JsonSerializer::CommitsFromJson(j);
  for (const auto& commit : *readCommits) {
    commitsRef_.push_back(commit);
  }

  return true;
}

void CommitsManager::Update() const {
  std::ofstream ofs(commitsFile_);
  if (!ofs) {
    return;
  }

  const auto repoJson = JsonSerializer::CommitsToJson(commitsRef_).dump(2);
  ofs << repoJson;
}
