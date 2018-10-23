#ifndef LLCE_PLATFORM_H
#define LLCE_PLATFORM_H

#include "consts.h"

namespace llce {

namespace platform {
    /// Namespace Functions ///

    bit8_t* allocBuffer( uint64_t pBufferLength, bit8_t* pBufferStart = nullptr );
    bool32_t deallocBuffer( bit8_t* pBuffer, uint64_t pBufferLength );

    void* dllLoadHandle( const char8_t* pDLLPath );
    bool32_t dllUnloadHandle( void* pDLLHandle, const char8_t* pDLLPath );
    void* dllLoadSymbol( void* pDLLHandle, const char8_t* pDLLSymbol );
}

}

#endif
