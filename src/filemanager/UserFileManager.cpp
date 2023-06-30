#include "filemanager/UserFileManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

using namespace nlohmann;

bool UserFileManager::Create() {
  std::ofstream file;
  if (!fs::exists(userFile_) || fs::is_empty(userFile_)) {
    file.open(userFile_);
  }
  if (!file.is_open()) return false;
  // default values if empty
  json j;
  j["email"] = "testuser@mail.com";
  j["password"] = "testpassword";

  file << j.dump(2) << "\n";
  return true;
}

bool UserFileManager::Read() {
  std::ifstream ifs(userFile_);
  if (!fs::exists(userFile_) || fs::is_empty(userFile_) || !ifs) {
    std::cout << "Email or password is empty. Using default values.\n";
    return false;
  }

  json json = json::parse(ifs);
  if (json.empty()) return false;
  email_ = json["email"];
  password_ = json["password"];

  return true;
}

void UserFileManager::Init() {
  if (!Read()) {
    Create();
    Read();
  }
}

std::string_view UserFileManager::Email() const { return email_; }

std::string_view UserFileManager::Password() const { return password_; }
