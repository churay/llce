#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cstring>
#include <iostream>
#include <fstream>

#include "sdllib.h"

#include "timer.h"
#include "memory.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( sdllib::state*, sdllib::input* );
typedef void (*render_f)( const sdllib::state*, const sdllib::input* );
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
    const uint64_t cBufferLength = MEGABYTE_BL( 1 );
    llce::memory mem( 1, &cBufferLength, cBufferAddress );

    sdllib::state* state = (sdllib::state*)mem.allocate( 0, sizeof(sdllib::state) ); {
        sdllib::state temp;
        memcpy( state, &temp, sizeof(sdllib::state) );

        state->box[0] = 0.0;
        state->box[1] = 0.0;
        state->box[2] = 0.1;
        state->box[3] = 0.1;
    }

    std::fstream recStateStream, recInputStream;
    const char8_t* cStateFilePath = "out/sdl_state.dat", * cInputFilePath = "out/sdl_input.dat";
    const ioflag_t cIOModeR = std::fstream::binary | std::fstream::in;
    const ioflag_t cIOModeW = std::fstream::binary | std::fstream::out | std::fstream::trunc;

    /// Initialize Input State ///

    sdllib::input rawInput;
    sdllib::input* input = &rawInput;

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

    /// Initialize Graphics Assets ///

    LLCE_ASSERT_ERROR(
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) >= 0,
        "SDL failed to initialize; " << SDL_GetError() );

    { // Initialize OpenGL Context //
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );

        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // double-buffer
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );

        SDL_GL_SetSwapInterval( 1 ); //vsync
    }

    int32_t windowWidth = 640, windowHeight = 480;
    SDL_Window* window = SDL_CreateWindow(
        "Loop-Live Code Editing",                   // Window Title
        SDL_WINDOWPOS_UNDEFINED,                    // Window X Position
        SDL_WINDOWPOS_UNDEFINED,                    // Window Y Position
        windowWidth,                                // Window Width
        windowHeight,                               // Window Height
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE ); // Window Flags
    LLCE_ASSERT_ERROR( window != nullptr,
        "SDL failed to create window instance; " << SDL_GetError() );

    SDL_GLContext glcontext = SDL_GL_CreateContext( window );
    LLCE_ASSERT_ERROR( glcontext != nullptr,
        "SDL failed to generate OpenGL context; " << SDL_GetError() );

    /// Update/Render Loop ///

    bool32_t isRunning = true;
    bool32_t isRecording = false, isReplaying = false;
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
                SDL_GetWindowSize( window, &windowWidth, &windowHeight );
            } else if( event.type == SDL_KEYDOWN ) {
                // NOTE(JRC): The keys processed here are those that are pressed
                // but not held. This type of processing is good for one-time
                // events (e.g. recording, replaying, etc.).
                SDL_Keycode pressedKey = event.key.keysym.sym;
                if( pressedKey == SDLK_q ) {
                    isRunning = false;
                } else if( pressedKey == SDLK_r ) {
                    if( !isRecording ) {
                        recStateStream.open( cStateFilePath, cIOModeW );
                        recStateStream.write( mem.buffer(), mem.length() );
                        recStateStream.close();

                        recInputStream.open( cInputFilePath, cIOModeW );
                    } else {
                        recInputStream.close();
                    }
                    isRecording = !isRecording;
                } else if( pressedKey == SDLK_t ) {
                    if( !isReplaying ) {
                        recStateStream.open( cStateFilePath, cIOModeR );
                        recInputStream.open( cInputFilePath, cIOModeR );
                        recInputStream.seekg( 0, std::ios_base::end );
                    } else {
                        recStateStream.close();
                        recInputStream.close();
                    }
                    isReplaying = !isReplaying;
                }
            }
        }

        // TODO(JRC): This is a bit weird for replaying because we allow intercepts
        // from any key before replacing all key presses with replay data. This is
        // good in some ways as it allows recordings to be excited, but it does
        // open the door for weird behavior like embedded recordings.
        if( isRecording ) {
            recInputStream.write( (bit8_t*)input->keys, sizeof(input->keys) );
        } if( isReplaying ) {
            if( recInputStream.peek() == EOF || recInputStream.eof() ) {
                recStateStream.seekg( 0 );
                recStateStream.read( mem.buffer(), mem.length() );
                recInputStream.seekg( 0 );
            }
            recInputStream.read( (bit8_t*)input->keys, sizeof(input->keys) );
        }

        LLCE_ASSERT_ERROR(
            currDylibModTime = llce::platform::fileStatModTime(sdllibFilePath),
            "Couldn't load library `" << sdllibFileName << "` stat data on step." );
        if( currDylibModTime != prevDylibModTime ) {
            llce::platform::fileWaitLock( sdllibFilePath );

            llce::platform::dllUnloadHandle( sdllibHandle, sdllibFileName );
            sdllibHandle = llce::platform::dllLoadHandle( sdllibFileName );
            updateFunction = (update_f)llce::platform::dllLoadSymbol( sdllibHandle, "update" );
            renderFunction = (render_f)llce::platform::dllLoadSymbol( sdllibHandle, "render" );
            LLCE_ASSERT_ERROR(
                sdllibHandle != nullptr && updateFunction != nullptr && renderFunction != nullptr,
                "Couldn't load library `" << sdllibFileName << "` symbols at " <<
                "simulation time " << simTimer.tt() << "." );

            prevDylibModTime = currDylibModTime;
        }

        glViewport( 0, 0, windowWidth, windowHeight );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f );

        glPushMatrix(); {
            updateFunction( state, input );
            renderFunction( state, input );
        } glPopMatrix();

        // TODO(JRC): Render text based on what's currently happening.
        SDL_GL_SwapWindow( window );

        simTimer.split( true );
        state->time += simTimer.ft();
    }

    /// Clean Up + Exit ///

    recStateStream.close();
    recInputStream.close();

    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
