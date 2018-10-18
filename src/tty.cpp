#include <stdio.h>
#include <string.h>

#include <fstream>

#include "ttylib.h"

#include "memory.h"
#include "timer.h"
#include "keyboard.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( ttylib::state*, ttylib::input* );
typedef void (*render_f)( const ttylib::state*, const ttylib::input* );
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

    ttylib::state* state = (ttylib::state*)mem.allocate( 0, sizeof(ttylib::state) ); {
        ttylib::state temp;
        memcpy( state, &temp, sizeof(ttylib::state) );
    }

    std::fstream recStateStream, recInputStream;
    const char8_t* cStateFilePath = "out/tty_state.dat", * cInputFilePath = "out/tty_input.dat";
    const ioflag_t cIOModeR = std::fstream::binary | std::fstream::in;
    const ioflag_t cIOModeW = std::fstream::binary | std::fstream::out | std::fstream::trunc;

    /// Initialize Input State ///

    ttylib::input rawInput;
    ttylib::input* input = &rawInput;

    llce::keyboard tty;
    LLCE_ASSERT_ERROR( tty.reading(), "Couldn't initialize keyboard input for process." );

    /// Load Dynamic Shared Library ///

    // TODO(JRC): Fix the calls to the 'stat' function so that they use the path
    // relative to the executable instead of the path relative to the run directory.
    // TODO(JRC): Create a function to calculate the full path of the dynamic
    // library so that it can be used easily in all platform functions.
    const char8_t* ttylibFileName = "ttylib.so";
    char8_t ttylibFilePath[MAXPATH_BL]; {
        strcpy( ttylibFilePath, ttylibFileName );
        LLCE_ASSERT_ERROR( llce::platform::libSearchRPath(ttylibFilePath),
            "Failed to find library " << ttylibFileName << " in dynamic path." );
    }

    void* ttylibHandle = llce::platform::dllLoadHandle( ttylibFileName );
    void* updateSymbol = llce::platform::dllLoadSymbol( ttylibHandle, "update" );
    void* renderSymbol = llce::platform::dllLoadSymbol( ttylibHandle, "render" );
    LLCE_ASSERT_ERROR(
        ttylibHandle != nullptr && updateSymbol != nullptr && renderSymbol != nullptr,
        "Couldn't load library `" << ttylibFileName << "` symbols on initialize." );

    update_f updateFunction = (update_f)updateSymbol;
    render_f renderFunction = (render_f)renderSymbol;

    int64_t prevDylibModTime, currDylibModTime;
    LLCE_ASSERT_ERROR(
        prevDylibModTime = currDylibModTime = llce::platform::fileStatModTime(ttylibFilePath),
        "Couldn't load library `" << ttylibFileName << "` stat data on initialize." );

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
        if( input->keys[llce::keyboard::keycode::r] ) {
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
            currDylibModTime = llce::platform::fileStatModTime(ttylibFilePath),
            "Couldn't load library `" << ttylibFileName << "` stat data on step." );
        if( currDylibModTime != prevDylibModTime ) {
            llce::platform::fileWaitLock( ttylibFilePath );

            llce::platform::dllUnloadHandle( ttylibHandle, ttylibFileName );
            ttylibHandle = llce::platform::dllLoadHandle( ttylibFileName );
            updateFunction = (update_f)llce::platform::dllLoadSymbol( ttylibHandle, "update" );
            renderFunction = (render_f)llce::platform::dllLoadSymbol( ttylibHandle, "render" );
            LLCE_ASSERT_ERROR(
                ttylibHandle != nullptr && updateFunction != nullptr && renderFunction != nullptr,
                "Couldn't load library `" << ttylibFileName << "` symbols at " <<
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
