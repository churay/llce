#ifndef LLCE_MEMORY_H
#define LLCE_MEMORY_H

#include "consts.h"

namespace llce {

class memory {
    public:

    /// Class Attributes ///

    const static uint32_t MAX_BLOCKS = 32;

    /// Constructors ///

    memory( uint64_t pBlockCount, const uint64_t* pBlockLengths, bit8_t* pBlockBase = nullptr );
    ~memory();

    /// Class Functions ///

    bit8_t* allocate( uint64_t pBufferIdx, uint64_t pAllocLength );

    bit8_t* buffer( uint64_t pBufferIdx = -1 ) const;
    uint64_t length( uint64_t pBufferIdx = -1 ) const;

    private:

    /// Class Fields ///

    bit8_t* mBuffer;
    uint64_t mBufferLength;

    uint64_t mBlockCount;
    bit8_t* mBlockBuffers[MAX_BLOCKS];
    uint64_t mBlockLengths[MAX_BLOCKS];
    uint64_t mBlockAllocs[MAX_BLOCKS];
};

}

#endif
