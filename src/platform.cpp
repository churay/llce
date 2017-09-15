#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "platform.h"

namespace llce {

int64_t platform::statSize( const char* pFilePath ) {
    int64_t fileSize = 0;

    // NOTE(JRC): According to the Unix documentation, the 'off_t' type is
    // flexible, but should be some form of signed integer.
    // (see: http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_types.h.html#tag_13_67)
    struct stat fileStatus;
    if( !stat(pFilePath, &fileStatus) ) {
        fileSize = static_cast<int64_t>( fileStatus.st_size );
    }

    return fileSize;
}


int64_t platform::statModTime( const char* pFilePath ) {
    int64_t fileModTime = 0;

    // NOTE(JRC): According to the C++ documentation, the 'time_t' type is
    // defined to be a 32-bit signed integer by most Unix implementations.
    // (see: http://en.cppreference.com/w/c/chrono/time)
    struct stat fileStatus;
    if( !stat(pFilePath, &fileStatus) ) {
        fileModTime = static_cast<int64_t>( fileStatus.st_mtime );
    }

    return fileModTime;
}


bool32_t platform::saveFullFile( const char* pFilePath, void* pBuffer, uint64_t pBufferLength ) {
    bool32_t saveSuccessful = false;

    FILE* file = fopen( pFilePath, "wb" );
    if( file != nullptr ) {
        if( fwrite(pBuffer, pBufferLength, 1, file) != 0 ) {
            saveSuccessful = true;
        }
        fclose( file );
    }

    return saveSuccessful;
}


bool32_t platform::loadFullFile( const char* pFilePath, void* pBuffer, uint64_t pBufferLength ) {
    bool32_t saveSuccessful = false;

    FILE* file = fopen( pFilePath, "rb" );
    if( file != nullptr ) {
        if( fread(pBuffer, pBufferLength, 1, file) != 0 ) {
            saveSuccessful = true;
        }
        fclose( file );
    }

    return saveSuccessful;
}


bool32_t platform::waitLockFile( const char* pFilePath ) {
    bool32_t waitSuccessful = false;

    char lockFilePath[MAXPATH_BL] = "";
    strcpy( lockFilePath, pFilePath );
    strcat( lockFilePath, ".lock" );

    int32_t lockFileHandle;
    if( (lockFileHandle = open(lockFilePath, O_RDWR)) >= 0 ) {
        // NOTE(JRC): The 'flock' function will block on a file if it has been
        // f'locked by another process; it's used here to wait on the flock and
        // then immediately continue processing.
        waitSuccessful = !flock( lockFileHandle, LOCK_EX ) && !flock( lockFileHandle, LOCK_UN );
        waitSuccessful &= !close( lockFileHandle );
    }

    return waitSuccessful;
}

}
