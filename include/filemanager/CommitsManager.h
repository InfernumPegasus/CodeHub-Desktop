#ifndef CODEHUB_COMMITSMANAGER_H
#define CODEHUB_COMMITSMANAGER_H

#include <string>
#include "commit/Commit.h"
#include "IFileManager.h"

class CommitsManager : public IFileManager {
public:
    CommitsManager(std::string commitsFile,
                   std::vector<Commit> *commits);

public:
    bool Create() override;

    bool Read() override;

    void Update() const;

private:
    std::string commitsFile_;

    std::vector<Commit> &commitsRef_;
};

#endif //CODEHUB_COMMITSMANAGER_H
