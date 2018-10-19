#ifndef LLCE_PLATFORM_H
#define LLCE_PLATFORM_H

#include "consts.h"

namespace llce {

namespace platform {
    /// Namespace Classes ///

    class path {
        public:

        /// Class Attributes ///

        // NOTE(JRC): This is the maximum byte length according to the author of eCryptfs.
        // (see: https://unix.stackexchange.com/a/32834)
        const static uint32_t MAX_LENGTH = 4096;
        const static char8_t SEP_CHAR = '/';

        /// Constructors ///

        path();
        path( const char8_t* pBuffer );

        /// Class Functions ///

        operator const char8_t*();

        bool32_t up();
        bool32_t dn( const char8_t* pChild );
        bool32_t dn( const path& pChild );

        private:

        /// Class Fields ///

        char8_t mBuffer[MAX_LENGTH];
        uint32_t mLength;
    };

    /// Namespace Functions ///

    bit8_t* allocBuffer( uint64_t pBufferLength, bit8_t* pBufferStart = nullptr );
    bool32_t deallocBuffer( bit8_t* pBuffer, uint64_t pBufferLength );

    int64_t fileStatSize( const char8_t* pFilePath );
    int64_t fileStatModTime( const char8_t* pFilePath );
    bool32_t fileWaitLock( const char8_t* pFilePath );

    void* dllLoadHandle( const char8_t* pDLLPath );
    bool32_t dllUnloadHandle( void* pDLLHandle, const char8_t* pDLLPath );
    void* dllLoadSymbol( void* pDLLHandle, const char8_t* pDLLSymbol );

    bool32_t exeGetAbsPath( char8_t* pFilePath );
    bool32_t libSearchRPath( char8_t* pFileName );
}

}

#endif
