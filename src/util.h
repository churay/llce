#ifndef LLCE_DYLIB_H
#define LLCE_DYLIB_H

#include "consts.h"

namespace llce {

namespace util {

bool32_t saveFullFile( const char* pFilename, void* pBuffer, uint64_t pBufferLength );
bool32_t loadFullFile( const char* pFilename, void* pBuffer, uint64_t pBufferLength );

}

}

#endif
