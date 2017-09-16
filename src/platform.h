#ifndef LLCE_PLATFORM_H
#define LLCE_PLATFORM_H

#include "consts.h"

namespace llce {

namespace platform {
    /// Namespace Functions ///

    int64_t statSize( const char* pFilePath );
    int64_t statModTime( const char* pFilePath );

    bool32_t saveFullFile( const char* pFilePath, void* pBuffer, uint64_t pBufferLength );
    bool32_t loadFullFile( const char* pFilePath, void* pBuffer, uint64_t pBufferLength );

    bool32_t waitLockFile( const char* pFilePath );

    bool32_t searchRPath( char* pFileName );
}

}

#endif
