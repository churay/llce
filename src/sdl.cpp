#include <SDL2/SDL.h>

#include <cstring>
#include <iostream>
#include <fstream>

#include "sdllib.h"

#include "timer.h"
#include "memory.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( sdllib::state*, sdllib::input* );
typedef void (*render_f)( SDL_Renderer*, const sdllib::state*, const sdllib::input* );
typedef std::ios_base::openmode ioflag_t;

int main() {
    /// Initialize Application Memory/State ///

    // NOTE(JRC): This base address was chosen by following the steps enumerated
    // in the 'doc/static_address.md' documentation file.
#ifdef LLCE_DEBUG
    bit8_t* const cBufferAddress = (bit8_t*)0x0000100000000000;
#else
    bit8_t* const cBufferAddress = nullptr;
#endif
    const uint64_t cStateBufferIdx = 0, cGraphicsBufferIdx = 1;
    const uint64_t cBufferLengths[] = { MEGABYTE_BL(1), MEGABYTE_BL(32) };
    const uint64_t cBufferCount = sizeof( cBufferLengths ) / sizeof( cBufferLengths[0] );
    llce::memory mem( cBufferCount, &cBufferLengths[0], cBufferAddress );

    const int32_t cWindowWidth = 640, cWindowHeight = 480;
    sdllib::state* state = (sdllib::state*)mem.allocate( cStateBufferIdx, sizeof(sdllib::state) ); {
        sdllib::state temp;
        memcpy( state, &temp, sizeof(sdllib::state) );

        state->texData = (uint32_t*)mem.allocate( cGraphicsBufferIdx,
            sizeof(uint32_t) * cWindowWidth * cWindowHeight );
        state->texBox[2] = cWindowWidth;
        state->texBox[3] = cWindowHeight;
        state->updated = true;
    }

    std::fstream recStateStream, recInputStream;
    const char8_t* cStateFilePath = "out/sdl_state.dat", * cInputFilePath = "out/sdl_input.dat";
    const ioflag_t cIOModeR = std::fstream::binary | std::fstream::in;
    const ioflag_t cIOModeW = std::fstream::binary | std::fstream::out | std::fstream::trunc;

    /// Initialize Input State ///

    sdllib::input rawInput;
    sdllib::input* input = &rawInput;

    LLCE_ASSERT_ERROR(
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) >= 0,
        "SDL failed to initialize; " << SDL_GetError() );

    /// Load Dynamic Shared Library ///

    const char8_t* sdllibFileName = "sdllib.so";
    char8_t sdllibFilePath[MAXPATH_BL]; {
        strcpy( sdllibFilePath, sdllibFileName );
        LLCE_ASSERT_ERROR( llce::platform::libSearchRPath(sdllibFilePath),
            "Failed to find library " << sdllibFileName << " in dynamic path." );
    }

    void* sdllibHandle = llce::platform::dllLoadHandle( sdllibFileName );
    void* updateSymbol = llce::platform::dllLoadSymbol( sdllibHandle, "update" );
    void* renderSymbol = llce::platform::dllLoadSymbol( sdllibHandle, "render" );
    LLCE_ASSERT_ERROR(
        sdllibHandle != nullptr && updateSymbol != nullptr && renderSymbol != nullptr,
        "Couldn't load library `" << sdllibFileName << "` symbols on initialize." );

    update_f updateFunction = (update_f)updateSymbol;
    render_f renderFunction = (render_f)renderSymbol;

    int64_t prevDylibModTime, currDylibModTime;
    LLCE_ASSERT_ERROR(
        prevDylibModTime = currDylibModTime = llce::platform::fileStatModTime(sdllibFilePath),
        "Couldn't load library `" << sdllibFileName << "` stat data on initialize." );

    /// Create an SDL Window ///

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

    // TODO(JRC): There isn't any guarantee that the texture handle will be
    // at the same address between instantiations of the application, so this
    // won't always work. This should really be fixed through a local texture
    // manager or something of that ilk.
    state->texHandle = (void*)SDL_CreateTexture(
        renderer,                            // Host Renderer
        SDL_PIXELFORMAT_RGBA8888,            // Pixel Format (RGBA, 8 bits each)
        SDL_TEXTUREACCESS_STREAMING,         // Texture Type (Streaming)
        state->texBox[2],                    // Texture Width
        state->texBox[3] );                  // Texture Height
    LLCE_ASSERT_ERROR( state->texHandle != nullptr,
        "SDL failed to create texture; " << SDL_GetError() );

    /// Update/Render Loop ///

    bool32_t isRunning = true, doRender = false;
    llce::timer simTimer( 60, llce::timer::type::fps );

    while( isRunning ) {
        simTimer.split();

        const uint8_t* keyboardState = SDL_GetKeyboardState( nullptr );
        std::memcpy( input->keys, keyboardState, sizeof(input->keys) );

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if( event.type == SDL_QUIT ) {
                isRunning = false;
            } else if( event.type == SDL_WINDOWEVENT  && (
                   event.window.event == SDL_WINDOWEVENT_RESIZED ||
                   event.window.event == SDL_WINDOWEVENT_EXPOSED) ) {
                doRender = true;
            } else if( event.type == SDL_KEYDOWN ) {
                // NOTE(JRC): The keys processed here are those that are pressed
                // but not held. This type of processing is good for one-time
                // events (e.g. recording, replaying, etc.).
                SDL_Keycode pressedKey = event.key.keysym.sym;
                if( pressedKey == SDLK_q ) {
                    isRunning = false;
                } else if( pressedKey == SDLK_r ) {
                    // TODO(JRC): Start recording.
                } else if( pressedKey == SDLK_t ) {
                    // TODO(JRC): Start replaying.
                }
            }
        }

        // TODO(JRC): Replaying, recording, llce behavior.

        updateFunction( state, input );
        if( doRender || state->updated ) {
            renderFunction( renderer, state, input );
        }

        simTimer.split( true );
    }

    /// Clean Up + Exit ///

    recStateStream.close();
    recInputStream.close();

    SDL_DestroyTexture( (SDL_Texture*)state->texHandle );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
