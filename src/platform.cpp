#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <elf.h>
#include <link.h>

#include "platform.h"

namespace llce {

void* platform::allocBuffer( uint64_t pBufferLength, void* pBufferBase ) {
    const int64_t cPermissionFlags = PROT_READ | PROT_WRITE;
    const int64_t cAllocFlags = MAP_ANONYMOUS | MAP_PRIVATE |
        ( (pBufferBase != nullptr) ? MAP_FIXED : 0 );

    // TODO(JRC): This whole piece is a bit messy and prone to race conditions
    // and thus should be cleaned up if possible.
    if( cAllocFlags & MAP_FIXED ) {
        const int64_t cPageSize = sysconf( _SC_PAGESIZE );

        bool8_t* bufferStart = (bool8_t*)pBufferBase;
        bool8_t* bufferEnd = (bool8_t*)pBufferBase + pBufferLength;
        unsigned char mincoreBuffer = false;

        bool32_t isBufferOccupied = false;
        for( bool8_t* pageIt = bufferStart; pageIt < bufferEnd; pageIt += cPageSize ) {
            isBufferOccupied |=
                mincore( pageIt, cPageSize, &mincoreBuffer ) == -1 && errno == ENOMEM;
        }

        LLCE_ASSERT_ERROR( !isBufferOccupied,
            "Unable to allocate buffer of length " << pBufferLength << " " <<
            "at base address " << pBufferBase << "; memory block is preoccupied." );
    }

    void* buffer = mmap(
        pBufferBase,             // Memory Start Address
        pBufferLength,           // Allocation Length (Bytes)
        cPermissionFlags,        // Data Permission Flags (Read/Write)
        cAllocFlags,             // Map Options (In-Memory, Private to Process)
        -1,                      // File Descriptor
        0 );                     // File Offset

    LLCE_ASSERT_ERROR( buffer != MAP_FAILED,
        "Unable to allocate buffer of length " << pBufferLength << "; " <<
        strerror(errno) );

    return buffer;
}


bool32_t platform::deallocBuffer( void* pBuffer, uint64_t pBufferLength ) {
    int64_t status = munmap( pBuffer, pBufferLength );
    return status == 0;
}


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


bool32_t platform::searchRPath( char* pFileName ) {
    // NOTE(JRC): The contents of this function heavily reference the system
    // implementation of the '<link.h>' dependency, which defines the C data
    // structures that interface with dynamic library symbol tables.
    // TODO(JRC): Extend this solution so that it loads using 'DT_RUNPATH' and
    // '$ORIGIN' like the built-in Unix run-time loading mechanism does.
    const char* procStringTable = nullptr;
    int32_t procRPathOffset = -1;

    for( const ElfW(Dyn)* dylibIter = _DYNAMIC; dylibIter->d_tag != DT_NULL; ++dylibIter ) {
        if( dylibIter->d_tag == DT_STRTAB ) {
            procStringTable = (const char*)( dylibIter->d_un.d_val );
        } else if( dylibIter->d_tag == DT_RPATH ) {
            procRPathOffset = (int32_t)( dylibIter->d_un.d_val );
        } 
    }

    char origFileName[MAXPATH_BL];
    strcpy( origFileName, pFileName );
    strcpy( pFileName, "" );

    if( procStringTable != nullptr && procRPathOffset >= 0 ) {
        const char* procRPath = procStringTable + procRPathOffset;

        for( const char* pathIter = procRPath; *pathIter != '\0'; pathIter = strchr(pathIter, ':') ) {
            strcat( pFileName, pathIter );
            strcat( pFileName, "/" );
            strcat( pFileName, origFileName );

            if( !access(pFileName, F_OK) ) { break; }
            else { strcpy( pFileName, "" ); }
        }
    }

    return strlen( pFileName ) != 0;
}

}
