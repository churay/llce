#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

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
    bit8_t* const cBufferAddress = (bit8_t*)0x0000100000000000;

    const uint64_t cStaticBufferIdx = 0, cDynamicBufferIdx = 1;
    const uint64_t cBufferLengths[] = { KILOBYTE_BL(1), KILOBYTE_BL(1) };
    const uint64_t cBufferCount = sizeof( cBufferLengths ) / sizeof( cBufferLengths[0] );

    llce::memory mem( cBufferCount, &cBufferLengths[0], cBufferAddress );

    dylib::state* state = (dylib::state*)mem.allocate( cStaticBufferIdx, sizeof(dylib::state) ); {
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

    // TODO(JRC): All of this dynamic library loading is Unix-specific and
    // should ultimately be moved into the 'platform' module.
    static auto loadLibrary = [] ( const char8_t* pLibraryName ) {
        void* libraryHandle = dlopen( pLibraryName, RTLD_NOW );
        const char8_t* libraryError = dlerror();

        LLCE_ASSERT_INFO( libraryHandle != nullptr,
            "Failed to load library `" << pLibraryName << "`: " << libraryError << "." );

        return libraryHandle;
    };

    static auto loadLibrarySymbol = [] (
            const void* pLibraryHandle, const char8_t* pSymbolName ) {
        void* symbolFunction = dlsym( const_cast<void*>(pLibraryHandle), pSymbolName );
        const char8_t* symbolError = dlerror();

        LLCE_ASSERT_INFO( symbolFunction != nullptr,
            "Failed to load symbol `" << pSymbolName << "`: " << symbolError << "." );

        return symbolFunction;
    };

    // TODO(JRC): Fix the calls to the 'stat' function so that they use the path
    // relative to the executable instead of the path relative to the run directory.
    // TODO(JRC): Create a function to calculate the full path of the dynamic
    // library so that it can be used easily in all platform functions.
    const char8_t* dylibFileName = "dylib.so";
    char8_t dylibFilePath[MAXPATH_BL]; {
        strcpy( dylibFilePath, dylibFileName );
        LLCE_ASSERT_ERROR( llce::platform::searchRPath(dylibFilePath),
            "Failed to find library " << dylibFileName << " in dynamic path." );
    }

    void* dylibHandle = loadLibrary( dylibFileName );
    void* updateSymbol = loadLibrarySymbol( dylibHandle, "update" );
    void* renderSymbol = loadLibrarySymbol( dylibHandle, "render" );
    LLCE_ASSERT_ERROR(
        dylibHandle != nullptr && updateSymbol != nullptr && renderSymbol != nullptr,
        "Couldn't load library `" << dylibFileName << "` symbols on initialize." );

    update_f updateFunction = (update_f)updateSymbol;
    render_f renderFunction = (render_f)renderSymbol;

    int64_t prevDylibModTime, currDylibModTime;
    LLCE_ASSERT_ERROR(
        prevDylibModTime = currDylibModTime = llce::platform::statModTime(dylibFilePath),
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
                recStateStream.write( mem.buffer(), mem.length() );
                recInputStream.seekg( 0 );
            }
            recInputStream.read( (bit8_t*)input->keys, sizeof(input->keys) );
        }

        LLCE_ASSERT_ERROR( currDylibModTime = llce::platform::statModTime(dylibFilePath),
            "Couldn't load library `" << dylibFileName << "` stat data on step." );
        if( currDylibModTime != prevDylibModTime ) {
            llce::platform::waitLockFile( dylibFilePath );

            dlclose( dylibHandle );
            dylibHandle = loadLibrary( dylibFileName );
            updateFunction = (update_f)loadLibrarySymbol( dylibHandle, "update" );
            renderFunction = (render_f)loadLibrarySymbol( dylibHandle, "render" );

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
