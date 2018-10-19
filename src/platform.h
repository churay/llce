#ifndef LLCE_PLATFORM_H
#define LLCE_PLATFORM_H

#include "consts.h"

namespace llce {

namespace platform {
    /// Namespace Constants ///

    extern const uint32_t MAX_PATH_BYTES;

    /// Namespace Functions ///

    bit8_t* allocBuffer( uint64_t pBufferLength, bit8_t* pBufferStart = nullptr );
    bool32_t deallocBuffer( bit8_t* pBuffer, uint64_t pBufferLength );

    int64_t fileStatSize( const char8_t* pFilePath );
    int64_t fileStatModTime( const char8_t* pFilePath );
    bool32_t fileWaitLock( const char8_t* pFilePath );

    bool32_t pathToChild( char8_t* pPath, const char8_t* pChild );
    bool32_t pathToParent( char8_t* pPath );

    void* dllLoadHandle( const char8_t* pDLLPath );
    bool32_t dllUnloadHandle( void* pDLLHandle, const char8_t* pDLLPath );
    void* dllLoadSymbol( void* pDLLHandle, const char8_t* pDLLSymbol );

    bool32_t exeGetAbsPath( char8_t* pFilePath );
    bool32_t libSearchRPath( char8_t* pFileName );
}

}

#endif
