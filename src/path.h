#ifndef LLCE_PATH_H
#define LLCE_PATH_H

#include "consts.h"

namespace llce {

namespace platform {

class path {
    public:

    /// Class Attributes ///

    // NOTE(JRC): This is the maximum byte length according to the author of eCryptfs.
    // (see: https://unix.stackexchange.com/a/32834)
    const static uint32_t MAX_LENGTH = 4096;

    const static char8_t EOS = '\0';
    const static char8_t DSEP = '/';
    constexpr static char8_t* DUP = nullptr;

    /// Constructors ///

    path();
    path( const char8_t* pBuffer );
    path( const uint32_t pArgCount, ... );

    /// Conversions ///

    operator const char8_t*() const;
    const char8_t* cstr() const;

    /// Class Functions ///

    bool32_t up( const uint32_t pLevels = 1 );
    bool32_t dn( const char8_t* pChild );

    bool32_t exists() const;
    int64_t size() const;
    int64_t modtime() const;

    bool32_t wait() const;

    /// External Functions ///

    friend path pathLockPath( const path& pBasePath );
    friend path exeBasePath();
    friend path libFindDLLPath( const char8_t* pLibName );

    private:

    /// Class Fields ///

    char8_t mBuffer[MAX_LENGTH];
    uint32_t mLength;
};

path pathLockPath( const path& pBasePath );
path exeBasePath();
path libFindDLLPath( const char8_t* pLibName );

}

}

#endif
