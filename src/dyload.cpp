#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <signal.h>
#include <errno.h>

#include "dylib.h"
#include "timer.h"
#include "platform.h"
#include "consts.h"

// TODO(JRC): This is really ugly and should be fixed is possible.
bool32_t isRunning = true;

typedef void (*update_f)( llce::state* );

int32_t main() {
    static auto processSignal = [] ( int32_t pSignal ) {
        isRunning = false;
    };
    signal( SIGINT, processSignal );

    /// Initialize Application Memory/State ///

    llce::memory mem;
    mem.permanentSize = MEGABYTE_BL( 64 );
    mem.transientSize = GIGABYTE_BL( 1 );

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

    llce::state* state = (llce::state*)mem.permanent;

    /// Initialize Input State ///

    llce::input input;

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
    LLCE_ASSERT_ERROR( dylibHandle != nullptr && updateSymbol != nullptr,
        "Couldn't load library `" << dylibFileName << "` symbols on initialize." );

    update_f updateFunction = (update_f)updateSymbol;

    int64_t prevDylibModTime, currDylibModTime;
    LLCE_ASSERT_ERROR( prevDylibModTime = currDylibModTime = llce::platform::statModTime(dylibFilePath),
        "Couldn't load library `" << dylibFileName << "` stat data on initialize." );

    /// Update Application ///

    printf( "Start!\n" );

    llce::timer simTimer( 60.0, llce::timer::type::fps );
    while( isRunning ) {
        simTimer.split();

        /*
        LLCE_ASSERT_ERROR( currDylibModTime = llce::platform::statModTime(dylibFilePath),
            "Couldn't load library `" << dylibFileName << "` stat data on step." );

        if( currDylibModTime != prevDylibModTime ) {
            llce::platform::waitLockFile( dylibFilePath );

            dlclose( dylibHandle );
            dylibHandle = loadLibrary( dylibFileName );
            updateFunction = (update_f)loadLibrarySymbol( dylibHandle, "update" );

            prevDylibModTime = currDylibModTime;
        }

        updateFunction( state );

        printf( "Current Value: %d (Elapsed Time: %f)  \r", state->xpos, simTimer.tt() );
        */

        tty.read( &input.keys[0] );
        printf( "Q Pressed?: %d", input.keys[llce::keyboard::keycode::q] );

        simTimer.split( true );
    }

    munmap( mem.permanent, mem.permanentSize );
    munmap( mem.transient, mem.transientSize );

    printf( "\nEnd!\n" );

    return 0;
}
