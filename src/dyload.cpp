#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>

#include "dylib.h"
#include "timer.h"
#include "consts.h"

// TODO(JRC): This is really ugly and should be fixed is possible.
bool32_t isRunning = true;

int32_t main() {
    static auto processSignal = [] ( int32_t pSignal ) { isRunning = false; };
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
        printf( "\nError!\n" );
        printf( "Permanent Status: %p", mem.permanent );
        printf( "Transient Status: %p", mem.transient );
        return -1;
    }

    mem.initialized = true;

    llce::state state;

    /// Update Application ///

    printf( "Start!\n" );

    llce::timer t( 60 );
    while( isRunning ) {
        t.split();

        update( &state );

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
