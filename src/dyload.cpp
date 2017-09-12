#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <signal.h>

#include "dylib.h"
#include "timer.h"
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
    mem.permanentSize = megabyte_bl( 64 );
    mem.transientSize = gigabyte_bl( 1 );

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

    if( mem.permanent == (void*)-1 || mem.transient == (void*)-1 ) {
        printf( "\nError: Couldn't allocate process memory!\n" );
        printf( "Permanent Status: %p", mem.permanent );
        printf( "Transient Status: %p", mem.transient );
        return 1;
    }

    mem.isInitialized = true;

    // TODO(JRC): Change this so that it's pointing at the memory allocated
    // for the application (in 'mem' variable).
    llce::state state;

    /// Load Dynamic Shared Library ///

    static auto loadLibrary = [] ( const char* pLibraryName ) {
        void* libraryHandle = dlopen( pLibraryName, RTLD_NOW );
        const char* libraryError = dlerror();
        if( libraryHandle == nullptr ) {
            printf( "Failed to Load Library %s: %s\n", pLibraryName, libraryError );
            return static_cast<void*>( nullptr );
        }

        return libraryHandle;
    };

    static auto loadLibrarySymbol = [] (
            const void* pLibraryHandle, const char* pSymbolName ) {
        void* symbolFunction = dlsym( const_cast<void*>(pLibraryHandle), pSymbolName );
        const char* symbolError = dlerror();
        if( symbolFunction == nullptr ) {
            printf( "Failed to Load Symbol %s: %s\n", pSymbolName, symbolError );
            return static_cast<void*>( nullptr );
        }

        return symbolFunction;
    };

    void* dylibHandle = loadLibrary( "dylib.so" );
    void* updateSymbol = loadLibrarySymbol( dylibHandle, "update" );
    if( dylibHandle == nullptr || updateSymbol == nullptr ) {
        printf( "Error: Couldn't load library at initialize!\n" );
        return 2;
    }
    update_f updateFunction = (update_f)updateSymbol;

    // TODO(JRC): Fix the calls to the 'stat' function so that they use the path
    // relative to the executable instead of the path relative to the run directory.
    struct stat prevDylibStatus, currDylibStatus;
    if( stat("bin/dylib.so", &prevDylibStatus) ) {
        printf( "Error: Couldn't load library timestamp data at initialize!\n" );
        return 3;
    }
    currDylibStatus = prevDylibStatus;

    /// Update Application ///

    printf( "Start!\n" );

    llce::timer simTimer( 60.0, llce::timer::type::fps );
    while( isRunning ) {
        simTimer.split();

        if( stat("bin/dylib.so", &currDylibStatus) ) {
            printf( "Error: Couldn't load library timestamp data at step!\n" );
            return 4;
        } else if( currDylibStatus.st_ctime != prevDylibStatus.st_ctime ) {
            // NOTE(JRC): 
            int32_t dylibFile = open( "bin/dylib.so", O_RDWR );
            flock( dylibFile, LOCK_EX );
            flock( dylibFile, LOCK_UN );
            close( dylibFile );

            printf("Loaded!\n");

            dlclose( dylibHandle );
            dylibHandle = loadLibrary( "dylib.so" );
            updateFunction = (update_f)loadLibrarySymbol( dylibHandle, "update" );

            prevDylibStatus = currDylibStatus;
        }

        updateFunction( &state );

        //printf( "Current Value: %d (Elapsed Time: %f)  \r", state.value, simTimer.tt() );

        simTimer.split( true );
    }

    munmap( mem.permanent, mem.permanentSize );
    munmap( mem.transient, mem.transientSize );

    printf( "\nEnd!\n" );

    return 0;
}
