#ifndef LLCE_PLATFORM_H
#define LLCE_PLATFORM_H

#include "consts.h"

namespace llce {

namespace platform {
    /// Namespace Functions ///

    bit8_t* allocBuffer( uint64_t pBufferLength, bit8_t* pBufferStart = nullptr );
    bool32_t deallocBuffer( bit8_t* pBuffer, uint64_t pBufferLength );

    int64_t statSize( const char8_t* pFilePath );
    int64_t statModTime( const char8_t* pFilePath );

    bool32_t saveFullFile( const char8_t* pFilePath, bit8_t* pBuffer, uint64_t pBufferLength );
    bool32_t loadFullFile( const char8_t* pFilePath, bit8_t* pBuffer, uint64_t pBufferLength );

    bool32_t waitLockFile( const char8_t* pFilePath );

    bool32_t searchRPath( char8_t* pFileName );
}

}

#endif
