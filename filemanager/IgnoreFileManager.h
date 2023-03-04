#ifndef CODEHUB_IGNOREFILEMANAGER_H
#define CODEHUB_IGNOREFILEMANAGER_H

#include <string>
#include <set>

class IgnoreFileManager {
public:
    IgnoreFileManager(std::string_view repositoryFolder,
                      std::string ignoreFile,
                      std::set<std::string> *ignoredFiles);

public:
    [[nodiscard]] bool CreateIgnoreFile() const;

    bool ReadIgnoreFile();

public:
    static bool CreateIgnoreFile(std::string_view path,
                                 const std::set<std::string> &files);

    static bool ReadIgnoreFile(std::string_view path,
                               std::set<std::string> &files);

private:
    std::string_view repositoryFolder_;
    std::string ignoreFile_;

    std::set<std::string> &ignoredFilesRef_;
};


#endif //CODEHUB_IGNOREFILEMANAGER_H
