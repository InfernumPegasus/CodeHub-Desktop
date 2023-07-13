#include "filemanager/CommitsManager.h"

#include <fstream>

#include "serializer/JsonSerializer.h"

CommitsManager::CommitsManager(fs::path commitsFile, types::Commits* commits)
    : commitsFile_(std::move(commitsFile)), commitsRef_(*commits) {}

bool CommitsManager::Create() {
  std::ofstream file(commitsFile_);
  return file.is_open();
}

bool CommitsManager::Read() {
  if (!std::filesystem::exists(commitsFile_) || std::filesystem::is_empty(commitsFile_)) {
    return false;
  }

  std::ifstream ifs(commitsFile_);
  if (!ifs) return true;

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
