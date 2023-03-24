#ifndef CODEHUB_IFILEMANAGER_H
#define CODEHUB_IFILEMANAGER_H


class IFileManager {
public:
    virtual bool Create() = 0;

    virtual bool Read() = 0;
};


#endif //CODEHUB_IFILEMANAGER_H
