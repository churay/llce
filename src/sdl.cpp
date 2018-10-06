#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>

#include "sdllib.h"

#include "timer.h"
#include "texture.h"
#include "consts.h"

typedef void (*update_f)( sdllib::state*, sdllib::input* );
typedef void (*render_f)( const sdllib::state*, const sdllib::input* );
typedef std::ios_base::openmode ioflag_t;

int main() {
    /*
    /// Initialize Application Memory/State ///

    // NOTE(JRC): This base address was chosen by following the steps enumerated
    // in the 'doc/static_address.md' documentation file.
#ifdef LLCE_DEBUG
    bit8_t* const cBufferAddress = (bit8_t*)0x0000100000000000;
#else
    bit8_t* const cBufferAddress = nullptr;
#endif
    // multiple buffers; one for state data, one for graphics data
    const uint64_t cBufferLength = MEGABYTE_BL( 1 );
    llce::memory mem( 1, &cBufferLength, cBufferAddress );

    sdllib::state* state = (sdllib::state*)mem.allocate( 0, sizeof(sdllib::state) ); {
        sdllib::state temp;
        memcpy( state, &temp, sizeof(sdllib::state) );
    }

    std::fstream recStateStream, recInputStream;
    const char8_t* cStateFilePath = "out/state.dat", * cInputFilePath = "out/input.dat";
    const ioflag_t cIOModeR = std::fstream::binary | std::fstream::in;
    const ioflag_t cIOModeW = std::fstream::binary | std::fstream::out | std::fstream::trunc;

    /// Initialize Input State ///

    sdllib::input rawInput;
    sdllib::input* input = &rawInput;
    */

    LLCE_ASSERT_ERROR(
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) >= 0,
        "SDL failed to initialize; " << SDL_GetError() );

    /// Create an SDL Window ///

    const int32_t cWindowWidth = 640, cWindowHeight = 480;

    SDL_Window* window = SDL_CreateWindow(
        "Loop-Live Code Editing",       // Window Title
        SDL_WINDOWPOS_UNDEFINED,        // Window X Position
        SDL_WINDOWPOS_UNDEFINED,        // Window Y Position
        cWindowWidth,                   // Window Width
        cWindowHeight,                  // Window Height
        SDL_WINDOW_RESIZABLE );         // Window Initialization Flags
    LLCE_ASSERT_ERROR( window != nullptr,
        "SDL failed to create window instance; " << SDL_GetError() );

    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, 0 );
    LLCE_ASSERT_ERROR( renderer != nullptr,
        "SDL failed to create window renderer; " << SDL_GetError() );

    /// Create and Load Texture ///

    llce::texture* texture = new llce::texture( renderer, cWindowWidth, cWindowHeight );
    auto loadTexture = [ &texture ] ( const int32_t xOff, const int32_t yOff ) {
        uint32_t* textureData = texture->mData;
        for( int y = 0; y < texture->mHeight; y++ ) {
            for( int x = 0; x < texture->mWidth; x++ ) {
                int i = x + y * texture->mWidth;
                textureData[i] = 0;
                textureData[i] |= (uint8_t)( 0x00 ) << 24;    // red
                textureData[i] |= (uint8_t)( y+yOff ) << 16;  // green
                textureData[i] |= (uint8_t)( x+xOff ) << 8;   // blue
                textureData[i] |= (uint8_t)( 0xFF ) << 0;     // alpha
            }
        }

        return texture->update();
    };

    /// Update/Render Loop ///

    int32_t xOffset = 0, yOffset = 0;

    bool32_t isRunning = true, doRender = false;
    llce::timer simTimer( 60, llce::timer::type::fps );

    while( isRunning ) {
        simTimer.split();

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if( event.type == SDL_QUIT ) {
                isRunning = false;
            } else if( event.type == SDL_WINDOWEVENT  && (
                   event.window.event == SDL_WINDOWEVENT_RESIZED ||
                   event.window.event == SDL_WINDOWEVENT_EXPOSED) ) {
                doRender = true;
            }
            // NOTE(JRC): Implement the following code in order to detect
            // when a key is pressed but not held.
            /*
            } else if( event.type == SDL_KEYDOWN ) {
                SDL_Keycode pressedKey = event.key.keysym.sym;
                if( pressedKey == SDLK_q ) {
                    isRunning = false;
                }
            }
            */
        }

        // NOTE(JRC): The movement numbers seem inverted because the values
        // being changed are the offsets for the underlying texture.  By
        // doing a positive offset in Y for example, we offset the texture
        // downward on the screen, giving the appearance of the window
        // "camera" going upwards.
        const uint8_t* keyboardState = SDL_GetKeyboardState( nullptr );
        if( keyboardState[SDL_SCANCODE_Q] ) {
            isRunning = false;
        } if( keyboardState[SDL_SCANCODE_W] ) {
            yOffset += 1;
            doRender = true;
        } if( keyboardState[SDL_SCANCODE_S] ) {
            yOffset -= 1;
            doRender = true;
        } if( keyboardState[SDL_SCANCODE_A] ) {
            xOffset += 1;
            doRender = true;
        } if( keyboardState[SDL_SCANCODE_D] ) {
            xOffset -= 1;
            doRender = true;
        }

        if( doRender ) {
            if( loadTexture(xOffset, yOffset) != 0 ) {
                std::cout << "SDL failed to load texture." << std::endl;
                isRunning = false;
            }

            SDL_RenderClear( renderer );
            SDL_RenderCopy( renderer, texture->mHandle, nullptr, nullptr );
            SDL_RenderPresent( renderer );
        }

        simTimer.split( true );
    }

    /// Clean Up + Exit ///

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
