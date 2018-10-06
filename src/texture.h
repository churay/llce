#ifndef LLCE_TEXTURE_H
#define LLCE_TEXTURE_H

#include <SDL2/SDL.h>

#include "consts.h"

namespace llce {

class texture {
    public:

    /// Constructors ///

    texture( SDL_Renderer* pHost, int32_t pWidth, int32_t pHeight );
    ~texture();

    int32_t update();

    /// Class Fields ///

    SDL_Renderer* mHost;
    SDL_Texture* mHandle;
    uint32_t* mData;

    int32_t mWidth, mHeight;
    int32_t mBytesPerPixel;
};

}

#endif
