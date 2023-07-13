#ifndef CODEHUB_COMMITSMANAGER_H
#define CODEHUB_COMMITSMANAGER_H

#include "IFileManager.h"
#include "commit/Commit.h"
#include "config/Types.h"

class CommitsManager : public IFileManager {
 public:
  CommitsManager(fs::path commitsFile, types::Commits* commits);

 public:
  bool Create() override;

  bool Read() override;

  void Update() const;

 private:
  const fs::path commitsFile_;

  types::Commits& commitsRef_;
};

#endif  // CODEHUB_COMMITSMANAGER_H
