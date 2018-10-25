#include <stdio.h>
#include <string.h>

#include <fstream>

#include "ttylib.h"

#include "memory.h"
#include "timer.h"
#include "keyboard.h"
#include "path.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( ttylib::state*, ttylib::input* );
typedef void (*render_f)( const ttylib::state*, const ttylib::input* );
typedef std::ios_base::openmode ioflag_t;
typedef llce::platform::path path_t;

int32_t main() {
    /// Initialize Application Memory/State ///

    // NOTE(JRC): This base address was chosen by following the steps enumerated
    // in the 'doc/static_address.md' documentation file.
#ifdef LLCE_DEBUG
    bit8_t* const cBufferAddress = (bit8_t*)0x0000100000000000;
#else
    bit8_t* const cBufferAddress = nullptr;
#endif
    const uint64_t cBufferLength = KILOBYTE_BL( 1 );
    llce::memory mem( 1, &cBufferLength, cBufferAddress );

    ttylib::state* state = (ttylib::state*)mem.allocate( 0, sizeof(ttylib::state) ); {
        ttylib::state temp;
        memcpy( state, &temp, sizeof(ttylib::state) );
    }

    std::fstream recStateStream, recInputStream;
    const ioflag_t cIOModeR = std::fstream::binary | std::fstream::in;
    const ioflag_t cIOModeW = std::fstream::binary | std::fstream::out | std::fstream::trunc;

    /// Initialize Input State ///

    ttylib::input rawInput;
    ttylib::input* input = &rawInput;

    llce::keyboard tty;
    LLCE_ASSERT_ERROR( tty.reading(), "Couldn't initialize keyboard input for process." );

    /// Find Project Paths ///

    const path_t cExePath = llce::platform::exeBasePath();
    LLCE_ASSERT_ERROR( cExePath.exists(),
        "Failed to find path to running executable." );

    const path_t cProjPath( 3, cExePath.cstr(), nullptr, nullptr );
    LLCE_ASSERT_ERROR( cProjPath.exists(),
        "Failed to find path to running executable." );

    const path_t cStateFilePath( 3, cProjPath.cstr(), "out", "sdl_state.dat" );
    const path_t cInputFilePath( 3, cProjPath.cstr(), "out", "sdl_input.dat" );

    /// Load Dynamic Shared Library ///

    const char8_t* cDLLFileName = "ttylib.so";
    const path_t cDLLPath = llce::platform::libFindDLLPath( cDLLFileName );
    LLCE_ASSERT_ERROR( cDLLPath.exists(),
        "Failed to find library " << cDLLFileName << " in dynamic path." );
    const path_t cDLLLockPath = llce::platform::pathLockPath( cDLLPath );

    void* ttyLibHandle = llce::platform::dllLoadHandle( cDLLPath );
    void* updateSymbol = llce::platform::dllLoadSymbol( ttyLibHandle, "update" );
    void* renderSymbol = llce::platform::dllLoadSymbol( ttyLibHandle, "render" );
    LLCE_ASSERT_ERROR(
        ttyLibHandle != nullptr && updateSymbol != nullptr && renderSymbol != nullptr,
        "Couldn't load library `" << cDLLFileName << "` symbols on initialize." );

    update_f updateFunction = (update_f)updateSymbol;
    render_f renderFunction = (render_f)renderSymbol;

    int64_t prevDylibModTime, currDylibModTime;
    LLCE_ASSERT_ERROR(
        prevDylibModTime = currDylibModTime = cDLLPath.modtime(),
        "Couldn't load library `" << cDLLFileName << "` stat data on initialize." );

    /// Update Application ///

    printf( "Start!\n" );

    bool32_t isRecording = false, isReplaying = false;
    bool32_t isRunning = true;
    llce::timer simTimer( 2.0, llce::timer::type::fps );

    while( isRunning ) {
        simTimer.split();

        tty.read( input->keys );
        if( input->keys[llce::keyboard::keycode::q] ) {
            isRunning = false;
        }
#ifdef LLCE_DEBUG
        if( input->keys[llce::keyboard::keycode::t] && !isRecording ) {
            if( !isReplaying ) {
                recStateStream.open( cStateFilePath, cIOModeR );
                recInputStream.open( cInputFilePath, cIOModeR );
                recInputStream.seekg( 0, std::ios_base::end );
            } else {
                recStateStream.close();
                recInputStream.close();
            }
            isReplaying = !isReplaying;
        } if( input->keys[llce::keyboard::keycode::r] && !isReplaying ) {
            if( !isRecording ) {
                recStateStream.open( cStateFilePath, cIOModeW );
                recStateStream.write( mem.buffer(), mem.length() );
                recStateStream.close();

                recInputStream.open( cInputFilePath, cIOModeW );
            } else {
                recInputStream.close();
            }
            isRecording = !isRecording;
        }
#endif

#ifdef LLCE_DEBUG
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
            currDylibModTime = cDLLPath.modtime(),
            "Couldn't load library `" << cDLLFileName << "` stat data on step." );
        if( currDylibModTime != prevDylibModTime ) {
            cDLLLockPath.wait();

            llce::platform::dllUnloadHandle( ttyLibHandle, cDLLFileName );
            ttyLibHandle = llce::platform::dllLoadHandle( cDLLFileName );
            updateFunction = (update_f)llce::platform::dllLoadSymbol( ttyLibHandle, "update" );
            renderFunction = (render_f)llce::platform::dllLoadSymbol( ttyLibHandle, "render" );
            LLCE_ASSERT_ERROR(
                ttyLibHandle != nullptr && updateFunction != nullptr && renderFunction != nullptr,
                "Couldn't load library `" << cDLLFileName << "` symbols at " <<
                "simulation time " << simTimer.tt() << "." );

            prevDylibModTime = currDylibModTime;
        }
#endif

        state->time = simTimer.tt();
        updateFunction( state, input );
        renderFunction( state, input );

        simTimer.split( true );
    }

    printf( "\nEnd!\n" );

    recStateStream.close();
    recInputStream.close();

    return 0;
}
