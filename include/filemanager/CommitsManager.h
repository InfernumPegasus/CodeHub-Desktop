#ifndef CODEHUB_COMMITSMANAGER_H
#define CODEHUB_COMMITSMANAGER_H

#include <string>

#include "IFileManager.h"
#include "commit/Commit.h"

class CommitsManager : public IFileManager {
 public:
  CommitsManager(std::string commitsFile, std::vector<Commit>* commits);

 public:
  bool Create() override;

  bool Read() override;

  void Update() const;

 private:
  const std::string commitsFile_;

  std::vector<Commit>& commitsRef_;
};

#endif  // CODEHUB_COMMITSMANAGER_H
