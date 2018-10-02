#ifndef LLCE_MEMORY_H
#define LLCE_MEMORY_H

#include "consts.h"

namespace llce {

class memory {
    public:

    /// Class Attributes ///

    const static uint32_t MAX_BLOCKS = 32;

    /// Constructors ///

    memory( uint64_t pBlockCount, const uint64_t* pBlockLengths, void* pBlockBase = nullptr );
    ~memory();

    /// Class Functions ///

    void* allocate( uint64_t pBufferIdx, uint64_t pAllocLength );

    void* buffer( uint64_t pBufferIdx = -1 ) const;
    uint64_t length( uint64_t pBufferIdx = -1 ) const;

    private:

    /// Class Fields ///

    void* mBuffer;
    uint64_t mBufferLength;

    uint64_t mBlockCount;
    void* mBlockBuffers[MAX_BLOCKS];
    uint64_t mBlockLengths[MAX_BLOCKS];
    uint64_t mBlockAllocs[MAX_BLOCKS];
};

}

#endif
