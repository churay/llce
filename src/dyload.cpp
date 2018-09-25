#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>

#include "dylib.h"
#include "timer.h"
#include "keyboard.h"
#include "platform.h"
#include "consts.h"

typedef void (*update_f)( llce::state*, llce::input* );
typedef void (*render_f)( const llce::state*, const llce::input* );

int32_t main() {
    /// Initialize Application Memory/State ///

    llce::memory mem;
    mem.permanentSize = MEGABYTE_BL( 64 );
    mem.transientSize = GIGABYTE_BL( 1 );

    // TODO(JRC): The base address should be somewhere outside of the application
    // space in the debug case to allow for loop-live code editing and null in
    // the application case so that it's allocated at a location the OS is happy with.

    // TODO(JRC): The mapped pieces of memory should be mapped to a static base
    // address so that pointers remain valid even if we load state from a file
    // during loop-live code editing.
    mem.permanent = mmap(
        nullptr,                         // Memory Start Address
        mem.permanentSize,               // Allocation Length (Bytes)
        PROT_READ | PROT_WRITE,          // Protection Flags (Read/Write)
        MAP_ANONYMOUS | MAP_PRIVATE,     // Map Options (In-Memory, Private to Process)
        -1,                              // File Descriptor
        0 );                             // File Offset
    mem.transient = mmap(
        nullptr,                         // Memory Start Address
        mem.transientSize,               // Allocation Length (Bytes)
        PROT_READ | PROT_WRITE,          // Protection Flags (Read/Write)
        MAP_ANONYMOUS | MAP_PRIVATE,     // Map Options (In-Memory, Private to Process)
        -1,                              // File Descriptor
        0 );                             // File Offset

    LLCE_ASSERT_ERROR( mem.permanent != (void*)-1 && mem.transient != (void*)-1,
        "Couldn't allocate process memory; " <<
        "permanent storage allocation failed with code " << (int64_t)mem.permanent << ", " <<
        "transient storage allocation failed with code " << (int64_t)mem.transient << "." );
    mem.isInitialized = true;

    llce::state* state = (llce::state*)( (char*)mem.permanent ); {
        llce::state temp;
        memcpy( state, &temp, sizeof(llce::state) );
    }

    /// Initialize Input State ///

    llce::input* input = (llce::input*)( (char*)mem.permanent + sizeof(llce::state) );

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

    munmap( mem.permanent, mem.permanentSize );
    munmap( mem.transient, mem.transientSize );

    printf( "\nEnd!\n" );

    return 0;
}
