#include <stdio.h>
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

    mem.initialized = true;

    // TODO(JRC): Change this so that it's pointing at the memory allocated
    // for the application (in 'mem' variable).
    llce::state state;

    /// Load Dynamic Shared Library ///

    static auto loadLibrarySymbol = [] (
            const char* pLibraryName, const char* pSymbolName ) {
        void* libraryHandle = dlopen( pLibraryName, RTLD_LAZY );
        const char* libraryError = dlerror();
        if( libraryHandle == nullptr ) {
            printf( "Failed to Load Library %s: %s\n", pLibraryName, libraryError );
            return static_cast<void*>( nullptr );
        }

        void* symbolFunction = dlsym( libraryHandle, pSymbolName );
        const char* symbolError = dlerror();
        if( symbolFunction == nullptr ) {
            printf( "Failed to Load Symbol %s: %s\n", pSymbolName, symbolError );
            return static_cast<void*>( nullptr );
        }

        return symbolFunction;
    };

    void* updateSymbol = loadLibrarySymbol( "dylib.so", "update" );
    if( updateSymbol == nullptr ) {
        printf( "\nError: Couldn't load library at initialize!\n" );
        return 2;
    }
    update_f updateFunction = (update_f)updateSymbol;

    /// Update Application ///

    printf( "Start!\n" );

    llce::timer t( 60 );
    while( isRunning ) {
        t.split();

        updateFunction( &state );

        // TODO(JRC): The criteria for switching should be if a file change
        // is detected on the file handle for the DLL (use 'stat' or equivalent).
        printf( "Current Value: %d\r", state.value );

        t.wait();
    }

    munmap( mem.permanent, mem.permanentSize );
    munmap( mem.transient, mem.transientSize );

    printf( "\nEnd!\n" );

    return 0;
}
