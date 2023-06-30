#ifndef CODEHUB_USERFILEMANAGER_H
#define CODEHUB_USERFILEMANAGER_H

#include "IFileManager.h"
#include "config/ConfigFiles.h"

class UserFileManager : public IFileManager {
 public:
  bool Create() override;

  bool Read() override;

  void Init() override;

 public:
  [[nodiscard]] std::string_view Email() const;

  [[nodiscard]] std::string_view Password() const;

 private:
  const std::string userFile_{GetHomeDirectory() + "/" + VCS_USER_FILE};

  std::string email_;
  std::string password_;
};

#endif  // CODEHUB_USERFILEMANAGER_H
