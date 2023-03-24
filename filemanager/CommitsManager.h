#ifndef CODEHUB_COMMITSMANAGER_H
#define CODEHUB_COMMITSMANAGER_H

#include <string>
#include "../commit/Commit.h"

class CommitsManager {
public:
    CommitsManager(std::string commitsFile,
                   std::vector<Commit> *commits);

public:
    [[nodiscard]] bool CreateCommitsFile() const;

    void UpdateCommitsFile() const;

    bool ReadCommitsFile();

private:
    std::string commitsFile_;

    std::vector<Commit> &commitsRef_;
};


#endif //CODEHUB_COMMITSMANAGER_H
