#ifndef CODEHUB_COMMITSMANAGER_H
#define CODEHUB_COMMITSMANAGER_H

#include <list>
#include <string>

#include "IFileManager.h"
#include "commit/Commit.h"

class CommitsManager : public IFileManager {
 public:
  CommitsManager(std::string commitsFile, std::list<Commit>* commits);

 public:
  bool Create() override;

  bool Read() override;

  void Update() const;

 private:
  const std::string commitsFile_;

  std::list<Commit>& commitsRef_;
};

#endif  // CODEHUB_COMMITSMANAGER_H
