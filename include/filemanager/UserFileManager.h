#ifndef CODEHUB_USERFILEMANAGER_H
#define CODEHUB_USERFILEMANAGER_H

#include "IFileManager.h"
#include "utils/ConfigFiles.h"

class UserFileManager : public IFileManager {
 public:
  ~UserFileManager() override = default;

  bool Create() override;

  bool Read() override;

 public:
  [[nodiscard]] std::string_view Email() const;

  [[nodiscard]] std::string_view Password() const;

 private:
  const fs::path userFile_{GetHomeDirectory() / VCS_CONFIG_FOLDER / VCS_USER_FILE};

  std::string email_;
  std::string password_;
};

#endif  // CODEHUB_USERFILEMANAGER_H
