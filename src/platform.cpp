#include <string.h>
#include <stdio.h>

#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>

#include "platform.h"

namespace llce {

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
