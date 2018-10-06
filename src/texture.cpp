#include <sys/mman.h>
#include <SDL2/SDL.h>

#include "texture.h"

namespace llce {

texture::texture( SDL_Renderer* pHost, int32_t pWidth, int32_t pHeight ) {
    mHost = pHost;
    mWidth = pWidth;
    mHeight = pHeight;

    mData = (uint32_t*)mmap(
        nullptr,                             // Memory Start Address
        sizeof(uint32_t) * mWidth * mHeight, // Allocation Length (Bytes)
        PROT_READ | PROT_WRITE,              // Protection Flags (Read/Write)
        MAP_ANONYMOUS | MAP_PRIVATE,         // Map Options (In-Memory, Private to Process)
        -1,                                  // File Descriptor
        0 );                                 // File Offset
    mHandle = SDL_CreateTexture(
        mHost,                               // Host Renderer
        SDL_PIXELFORMAT_RGBA8888,            // Pixel Format (RGBA, 8 bits each)
        SDL_TEXTUREACCESS_STREAMING,         // Texture Type (Streaming)
        mWidth,                              // Texture Width
        mHeight );                           // Texture Height
}


texture::~texture() {
    munmap( (void*)mData, sizeof(uint32_t) * mWidth * mHeight );
    SDL_DestroyTexture( mHandle );
}


int32_t texture::update() {
    return SDL_UpdateTexture( mHandle, nullptr,
        (void*)mData, sizeof(uint32_t)*mWidth );
}


}
