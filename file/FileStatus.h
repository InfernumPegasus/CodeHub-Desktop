#ifndef CODEHUB_FILESTATUS_H
#define CODEHUB_FILESTATUS_H

enum class FileStatus {
    Unknown = -1,   // Should never be
    Created = 0,
    Modified = 1,
    Deleted = 2,
};


#endif //CODEHUB_FILESTATUS_H
