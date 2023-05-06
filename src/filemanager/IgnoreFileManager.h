#ifndef CODEHUB_IGNOREFILEMANAGER_H
#define CODEHUB_IGNOREFILEMANAGER_H

#include <string>
#include <unordered_set>
#include "IFileManager.h"

class IgnoreFileManager : public IFileManager {
public:
    IgnoreFileManager(std::string_view repositoryFolder,
                      std::string ignoreFile,
                      std::unordered_set<std::string> *ignoredFiles);

public:
    bool Create() override;

    bool Read() override;

private:
    std::string_view repositoryFolder_;
    std::string ignoreFile_;

    std::unordered_set<std::string> &ignoredFilesRef_;
};

#endif //CODEHUB_IGNOREFILEMANAGER_H
