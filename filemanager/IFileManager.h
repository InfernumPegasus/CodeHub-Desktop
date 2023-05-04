#ifndef CODEHUB_IFILEMANAGER_H
#define CODEHUB_IFILEMANAGER_H

#include <pwd.h>
#include <unistd.h>
#include <string>

class IFileManager {
public:
    static std::string GetHomeDirectory() {
        const auto dir = std::getenv("HOME");
        return dir == nullptr ? dir : getpwuid(getuid())->pw_dir;
    }

    virtual bool Create() = 0;

    virtual bool Read() = 0;

    virtual void Init() {
        if (!Read() &&
            Create() &&
            Read()) {}
    }
};


#endif //CODEHUB_IFILEMANAGER_H
