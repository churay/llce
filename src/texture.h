#ifndef LLCE_TEXTURE_H
#define LLCE_TEXTURE_H

#include <SDL2/SDL.h>

namespace llce {

class texture {
    public:

    /// Constructors ///

    texture( SDL_Renderer* pHost, size_t pWidth, size_t pHeight );
    ~texture();

    int update();

    /// Class Fields ///

    SDL_Renderer* mHost;
    SDL_Texture* mHandle;
    uint32_t* mData;

    size_t mWidth, mHeight;
    size_t mBytesPerPixel;
};

}

#endif
