#include <stdio.h>

#include "util.h"

namespace llce {

bool32_t util::saveFullFile( const char* pFilename, void* pBuffer, uint64_t pBufferLength ) {
    bool32_t saveSuccessful = false;

    FILE* file = fopen( pFilename, "wb" );
    if( file != nullptr ) {
        if( fwrite(pBuffer, pBufferLength, 1, file) != 0 ) {
            saveSuccessful = true;
        }
        fclose( file );
    }

    return saveSuccessful;
}


bool32_t loadFullFile( const char* pFilename, void* pBuffer, uint64_t pBufferLength ) {
    bool32_t saveSuccessful = false;

    FILE* file = fopen( pFilename, "rb" );
    if( file != nullptr ) {
        if( fread(pBuffer, pBufferLength, 1, file) != 0 ) {
            saveSuccessful = true;
        }
        fclose( file );
    }

    return saveSuccessful;
}

}
