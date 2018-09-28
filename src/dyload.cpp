#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "dylib.h"
#include "memory.h"
#include "timer.h"
#include "keyboard.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( dylib::state*, dylib::input* );
typedef void (*render_f)( const dylib::state*, const dylib::input* );

int32_t main() {
    /// Initialize Application Memory/State ///

    // NOTE(JRC): This base address was chosen by following the steps enumerated
    // in the 'doc/static_address.md' documentation file.
    void* cBufferAddress = (void*)0x0000100000000000;

    const uint64_t cStaticBufferIdx = 0, cDynamicBufferIdx = 1;
    const uint64_t cBufferLengths[] = { MEGABYTE_BL(64), GIGABYTE_BL(1) };
    const uint64_t cBufferCount = sizeof( cBufferLengths ) / sizeof( cBufferLengths[0] );

    llce::memory mem( cBufferCount, &cBufferLengths[0], cBufferAddress );

    dylib::state* state = (dylib::state*)mem.allocate( cStaticBufferIdx, sizeof(dylib::state) ); {
        dylib::state temp;
        memcpy( state, &temp, sizeof(dylib::state) );
    }

    /// Initialize Input State ///

    dylib::input* input = (dylib::input*)mem.allocate( cStaticBufferIdx, sizeof(dylib::input) );

    llce::keyboard tty;
    LLCE_ASSERT_ERROR( tty.reading(), "Couldn't initialize keyboard input for process." );

    /// Load Dynamic Shared Library ///

    static auto loadLibrary = [] ( const char* pLibraryName ) {
        void* libraryHandle = dlopen( pLibraryName, RTLD_NOW );
        const char* libraryError = dlerror();

        LLCE_ASSERT_INFO( libraryHandle != nullptr,
            "Failed to load library `" << pLibraryName << "`: " << libraryError << "." );

        return libraryHandle;
    };

    static auto loadLibrarySymbol = [] (
            const void* pLibraryHandle, const char* pSymbolName ) {
        void* symbolFunction = dlsym( const_cast<void*>(pLibraryHandle), pSymbolName );
        const char* symbolError = dlerror();

        LLCE_ASSERT_INFO( symbolFunction != nullptr,
            "Failed to load symbol `" << pSymbolName << "`: " << symbolError << "." );

        return symbolFunction;
    };

    // TODO(JRC): Fix the calls to the 'stat' function so that they use the path
    // relative to the executable instead of the path relative to the run directory.
    // TODO(JRC): Create a function to calculate the full path of the dynamic
    // library so that it can be used easily in all platform functions.
    const char* dylibFileName = "dylib.so";
    char dylibFilePath[MAXPATH_BL]; {
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
                // TODO(JRC): Start the recording by outputting all of the state at this
                // moment and start recording the input.
                // NOTE(JRC): We can continually write to the file if we keep the handle
                // open as writes will continously append to the end of the file (the
                // current location of the file pointer).
                // saveFullFile( "out/state.dat", mem->, sizeof() );
            } else {
                // TODO(JRC): Finish the recording and then start the playback.
                // TODO(JRC): In the playback, make sure to loop once the end of the
                // recorded input file is reached.
                // loadFullFile( "out/state.dat", mem->, sizeof() );
            }
            isRecording = !isRecording;
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

    return 0;
}
