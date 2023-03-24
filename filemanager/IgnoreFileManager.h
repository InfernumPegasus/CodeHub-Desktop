#ifndef CODEHUB_IGNOREFILEMANAGER_H
#define CODEHUB_IGNOREFILEMANAGER_H

#include <string>
#include <unordered_set>

class IgnoreFileManager {
public:
    IgnoreFileManager(std::string_view repositoryFolder,
                      std::string ignoreFile,
                      std::unordered_set<std::string> *ignoredFiles);

public:
    [[nodiscard]] bool CreateIgnoreFile() const;

    bool ReadIgnoreFile();

private:
    std::string_view repositoryFolder_;
    std::string ignoreFile_;

    std::unordered_set<std::string> &ignoredFilesRef_;
};


#endif //CODEHUB_IGNOREFILEMANAGER_H
