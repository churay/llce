#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <dlfcn.h>
#include <elf.h>
#include <link.h>

#include "platform.h"

namespace llce {

/// platform::path class ///

platform::path::path() {
    mLength = 0;
    mBuffer[mLength] = '\0';
}


platform::path::path( const char8_t* pBuffer ) {
    uint32_t inLength = 0;
    for( const char8_t* pItr = pBuffer; *pItr != '\0'; pItr++, inLength++ ) {}

    memcpy( &mBuffer[0], pBuffer, inLength );
    mLength = inLength;
    mBuffer[mLength] = '\0';
}


platform::path::operator const char8_t*() {
    return &mBuffer[0];
}


bool32_t platform::path::up() {
    bool32_t success = true;

    char8_t* pathItr = nullptr;
    for( pathItr = &mBuffer[0]; *pathItr != '\0'; pathItr++ ) {}
    for( ; pathItr > &mBuffer[0] && *pathItr != platform::path::SEP_CHAR; pathItr-- ) {}

    bool32_t hasPathParent = pathItr > &mBuffer[0];
    success &= hasPathParent;
    if( !hasPathParent ) {
        LLCE_ASSERT_INFO( false,
            "Cannot find parent to invalid path `" << &mBuffer[0] << "`." );
    } else {
        *pathItr = '\0';
        mLength = pathItr - &mBuffer[0];
    }

    return success;
}


bool32_t platform::path::dn( const char8_t* pChild ) {
    bool32_t success = true;

    uint32_t childLength = 0;
    for( const char8_t* pItr = pChild; *pItr != '\0'; pItr++, childLength++ ) {}

    bool32_t isPathOverflowed = mLength + childLength + 1 > path::MAX_LENGTH;
    success &= !isPathOverflowed;

    if( isPathOverflowed ) {
        LLCE_ASSERT_INFO( false,
            "Cannot find child `" << pChild << "` of extended path `" << &mBuffer[0] << "`." );
    } else {
        mBuffer[mLength] = path::SEP_CHAR;
        memcpy( &mBuffer[mLength + 1], pChild, childLength );
        mLength += 1 + childLength;
    }

    return success;
}


bool32_t platform::path::exists() const {
    return !access( &mBuffer[0], F_OK );
}


int64_t platform::path::size() const {
    int64_t fileSize = 0;

    // NOTE(JRC): According to the Unix documentation, the 'off_t' type is
    // flexible, but should be some form of signed integer.
    // (see: http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_types.h.html#tag_13_67)
    struct stat fileStatus;
    if( !stat(&mBuffer[0], &fileStatus) ) {
        fileSize = static_cast<int64_t>( fileStatus.st_size );
    }

    LLCE_ASSERT_INFO( fileSize > 0,
        "Failed to read size of file " << &mBuffer[0] << "; " <<
        strerror(errno) );

    return fileSize;
}


int64_t platform::path::modtime() const {
    int64_t fileModTime = 0;

    // NOTE(JRC): According to the C++ documentation, the 'time_t' type is
    // defined to be a 32-bit signed integer by most Unix implementations.
    // (see: http://en.cppreference.com/w/c/chrono/time)
    struct stat fileStatus;
    if( !stat(&mBuffer[0], &fileStatus) ) {
        fileModTime = static_cast<int64_t>( fileStatus.st_mtime );
    }

    LLCE_ASSERT_INFO( fileModTime > 0,
        "Failed to read mod time of file at path " << &mBuffer[0] << "; " <<
        strerror(errno) );

    return fileModTime;
}


bool32_t platform::path::wait() const {
    bool32_t waitSuccessful = false;

    int32_t fileHandle;
    if( (fileHandle = open(&mBuffer[0], O_RDWR)) >= 0 ) {
        // NOTE(JRC): The 'flock' function will block on a file if it has been
        // f'locked by another process; it's used here to wait on the flock and
        // then immediately continue processing.
        waitSuccessful = !flock( fileHandle, LOCK_EX ) && !flock( fileHandle, LOCK_UN );
        waitSuccessful &= !close( fileHandle );
    }

    LLCE_ASSERT_INFO( waitSuccessful,
        "Failed to pwait for file at path " << &mBuffer[0] << "; " <<
        strerror(errno) );

    return waitSuccessful;
}


platform::path platform::path::lock() const {
    const char8_t* lockSuffix = ".lock";

    path lockPath = *this;
    for( const char8_t* pItr = lockSuffix; *pItr != '\0'; pItr++ ) {
        lockPath.mBuffer[lockPath.mLength++] = *pItr;
    }
    lockPath.mBuffer[lockPath.mLength] = '\0';

    return lockPath;
}


platform::path platform::path::toRunningExe() {
    path exePath;
    int64_t status = readlink( "/proc/self/exe", &exePath.mBuffer[0],
        platform::path::MAX_LENGTH );

    if( status <= 0 ) {
        exePath.mBuffer[0] = '\0';
        exePath.mLength = 0;
        LLCE_ASSERT_INFO( false,
            "Failed to retrieve the path to the running executable; " <<
            strerror(errno) );
    } else {
        exePath.mLength = status;
    }

    return exePath;
}


platform::path platform::path::toDynamicLib( const char8_t* pLibName ) {
    path libPath;

    // NOTE(JRC): The contents of this function heavily reference the system
    // implementation of the '<link.h>' dependency, which defines the C data
    // structures that interface with dynamic library symbol tables.
    // TODO(JRC): Extend this solution so that it loads using 'DT_RUNPATH' and
    // '$ORIGIN' like the built-in Unix run-time loading mechanism does.
    const char8_t* procStringTable = nullptr;
    int32_t procRPathOffset = -1;

    for( const ElfW(Dyn)* dylibIter = _DYNAMIC; dylibIter->d_tag != DT_NULL; ++dylibIter ) {
        if( dylibIter->d_tag == DT_STRTAB ) {
            procStringTable = (const char8_t*)( dylibIter->d_un.d_val );
        } else if( dylibIter->d_tag == DT_RPATH ) {
            procRPathOffset = (int32_t)( dylibIter->d_un.d_val );
        }
    }

    const char8_t* procRPath = ( procStringTable != nullptr && procRPathOffset >= 0 ) ?
        procStringTable + procRPathOffset : nullptr;
    for( const char8_t* pathIter = procRPath;
            pathIter != nullptr && *pathIter != '\0' && !libPath.exists();
            pathIter = strchr(pathIter, ':') ) {
        libPath.mLength = 0;

        for( const char8_t* pItr = pathIter; *pItr != '\0'; pItr++ ) {
            libPath.mBuffer[libPath.mLength++] = *pItr;
        }
        libPath.mBuffer[libPath.mLength++] = platform::path::SEP_CHAR;
        for( const char8_t* pItr = pLibName; *pItr != '\0'; pItr++ ) {
            libPath.mBuffer[libPath.mLength++] = *pItr;
        }

        libPath.mBuffer[libPath.mLength] = '\0';
    }

    LLCE_ASSERT_INFO( libPath.exists(),
        "Failed to find `" << pLibName << "` in the executable's dynamic path; " <<
        strerror(errno) );

    return libPath;
}

/// platform functions ///

bit8_t* platform::allocBuffer( uint64_t pBufferLength, bit8_t* pBufferBase ) {
    const int64_t cPermissionFlags = PROT_READ | PROT_WRITE;
    const int64_t cAllocFlags = MAP_ANONYMOUS | MAP_PRIVATE |
        ( (pBufferBase != nullptr) ? MAP_FIXED : 0 );

    // TODO(JRC): This whole piece is a bit messy and prone to race conditions
    // and thus should be cleaned up if possible.
    if( cAllocFlags & MAP_FIXED ) {
        const int64_t cPageSize = sysconf( _SC_PAGESIZE );
        uchar8_t mincoreBuffer = false;

        bool32_t isBufferOccupied = false;
        for( bit8_t* pageIt = pBufferBase; pageIt < pBufferBase + pBufferLength; pageIt += cPageSize ) {
            isBufferOccupied |= !(
                mincore( pageIt, cPageSize, &mincoreBuffer ) == -1 &&
                errno == ENOMEM );
        }

        LLCE_ASSERT_INFO( !isBufferOccupied,
            "Allocation of buffer of length " << pBufferLength << " " <<
            "at base address " << pBufferBase << " will cause eviction of one "
            "or more existing memory blocks." );
    }

    bit8_t* buffer = (bit8_t*)mmap(
        pBufferBase,             // Memory Start Address
        pBufferLength,           // Allocation Length (Bytes)
        cPermissionFlags,        // Data Permission Flags (Read/Write)
        cAllocFlags,             // Map Options (In-Memory, Private to Process)
        -1,                      // File Descriptor
        0 );                     // File Offset

    LLCE_ASSERT_INFO( buffer != (bit8_t*)MAP_FAILED,
        "Unable to allocate buffer of length " << pBufferLength << " " <<
        "at base address " << pBufferBase << "; " << strerror(errno) );

    return buffer;
}


bool32_t platform::deallocBuffer( bit8_t* pBuffer, uint64_t pBufferLength ) {
    int64_t status = munmap( pBuffer, pBufferLength );

    LLCE_ASSERT_INFO( status == 0,
        "Deallocation of buffer of length " << pBufferLength << " " <<
        "at base address " << pBuffer << " failed; possible memory corruption." );

    return status == 0;
}


void* platform::dllLoadHandle( const char8_t* pDLLPath ) {
    void* libraryHandle = dlopen( pDLLPath, RTLD_NOW );
    const char8_t* libraryError = dlerror();

    LLCE_ASSERT_INFO( libraryHandle != nullptr,
        "Failed to load library `" << pDLLPath << "`: " << libraryError );

    return libraryHandle;
}


bool32_t platform::dllUnloadHandle( void* pDLLHandle, const char8_t* pDLLPath ) {
    int64_t status = dlclose( pDLLHandle );
    const char8_t* libraryError = dlerror();

    LLCE_ASSERT_INFO( status == 0,
        "Failed to unload library `" << pDLLPath << "`; " << libraryError );

    return status == 0;
}


void* platform::dllLoadSymbol( void* pDLLHandle, const char8_t* pDLLSymbol ) {
    void* symbolFunction = dlsym( const_cast<void*>(pDLLHandle), pDLLSymbol );
    const char8_t* symbolError = dlerror();

    LLCE_ASSERT_INFO( symbolFunction != nullptr,
        "Failed to load symbol `" << pDLLSymbol << "`: " << symbolError );

    return symbolFunction;
}

}
