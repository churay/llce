#include <stdio.h>
#include <string.h>

#include <fstream>

#include "dylib.h"

#include "memory.h"
#include "timer.h"
#include "keyboard.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( dylib::state*, dylib::input* );
typedef void (*render_f)( const dylib::state*, const dylib::input* );
typedef std::ios_base::openmode ioflag_t;

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

    dylib::state* state = (dylib::state*)mem.allocate( 0, sizeof(dylib::state) ); {
        dylib::state temp;
        memcpy( state, &temp, sizeof(dylib::state) );
    }

    std::fstream recStateStream, recInputStream;
    const char8_t* cStateFilePath = "out/state.dat", * cInputFilePath = "out/input.dat";
    const ioflag_t cIOModeR = std::fstream::binary | std::fstream::in;
    const ioflag_t cIOModeW = std::fstream::binary | std::fstream::out | std::fstream::trunc;

    /// Initialize Input State ///

    dylib::input rawInput;
    dylib::input* input = &rawInput;

    llce::keyboard tty;
    LLCE_ASSERT_ERROR( tty.reading(), "Couldn't initialize keyboard input for process." );

    /// Load Dynamic Shared Library ///

    // TODO(JRC): Fix the calls to the 'stat' function so that they use the path
    // relative to the executable instead of the path relative to the run directory.
    // TODO(JRC): Create a function to calculate the full path of the dynamic
    // library so that it can be used easily in all platform functions.
    const char8_t* dylibFileName = "dylib.so";
    char8_t dylibFilePath[MAXPATH_BL]; {
        strcpy( dylibFilePath, dylibFileName );
        LLCE_ASSERT_ERROR( llce::platform::libSearchRPath(dylibFilePath),
            "Failed to find library " << dylibFileName << " in dynamic path." );
    }

    void* dylibHandle = llce::platform::dllLoadHandle( dylibFileName );
    void* updateSymbol = llce::platform::dllLoadSymbol( dylibHandle, "update" );
    void* renderSymbol = llce::platform::dllLoadSymbol( dylibHandle, "render" );
    LLCE_ASSERT_ERROR(
        dylibHandle != nullptr && updateSymbol != nullptr && renderSymbol != nullptr,
        "Couldn't load library `" << dylibFileName << "` symbols on initialize." );

    update_f updateFunction = (update_f)updateSymbol;
    render_f renderFunction = (render_f)renderSymbol;

    int64_t prevDylibModTime, currDylibModTime;
    LLCE_ASSERT_ERROR(
        prevDylibModTime = currDylibModTime = llce::platform::fileStatModTime(dylibFilePath),
        "Couldn't load library `" << dylibFileName << "` stat data on initialize." );

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
        } if( input->keys[llce::keyboard::keycode::r] ) {
            if( !isRecording ) {
                recStateStream.open( cStateFilePath, cIOModeW );
                recStateStream.write( mem.buffer(), mem.length() );
                recStateStream.close();

                recInputStream.open( cInputFilePath, cIOModeW );
            } else {
                recInputStream.close();
            }
            isRecording = !isRecording;
        } if( input->keys[llce::keyboard::keycode::t] ) {
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
            currDylibModTime = llce::platform::fileStatModTime(dylibFilePath),
            "Couldn't load library `" << dylibFileName << "` stat data on step." );
        if( currDylibModTime != prevDylibModTime ) {
            llce::platform::fileWaitLock( dylibFilePath );

            llce::platform::dllUnloadHandle( dylibHandle, dylibFileName );
            dylibHandle = llce::platform::dllLoadHandle( dylibFileName );
            updateFunction = (update_f)llce::platform::dllLoadSymbol( dylibHandle, "update" );
            renderFunction = (render_f)llce::platform::dllLoadSymbol( dylibHandle, "render" );
            LLCE_ASSERT_ERROR(
                dylibHandle != nullptr && updateFunction != nullptr && renderFunction != nullptr,
                "Couldn't load library `" << dylibFileName << "` symbols at " <<
                "simulation time " << simTimer.tt() << "." );

            prevDylibModTime = currDylibModTime;
        }

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
