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

        /// Conversions ///

        operator const char8_t*();

        /// Class Functions ///

        bool32_t up();
        bool32_t dn( const char8_t* pChild );

        bool32_t exists() const;
        int64_t size() const;
        int64_t modtime() const;

        path lock() const;
        bool32_t wait() const;

        // TODO(JRC): This is a really suboptimal place for these functions, but
        // it's the only place they can exist while freely accessing member
        // variables of the 'path' class. Ultimately, this should either become
        // a struct or some other solution for 'friending' these functions needs
        // to be discovered.
        static path toRunningExe();
        static path toDynamicLib( const char8_t* pLibName );

        private:

        /// Class Fields ///

        char8_t mBuffer[MAX_LENGTH];
        uint32_t mLength;
    };

    /// Namespace Functions ///

    bit8_t* allocBuffer( uint64_t pBufferLength, bit8_t* pBufferStart = nullptr );
    bool32_t deallocBuffer( bit8_t* pBuffer, uint64_t pBufferLength );

    void* dllLoadHandle( const char8_t* pDLLPath );
    bool32_t dllUnloadHandle( void* pDLLHandle, const char8_t* pDLLPath );
    void* dllLoadSymbol( void* pDLLHandle, const char8_t* pDLLSymbol );
}

}

#endif
