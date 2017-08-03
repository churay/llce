#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <signal.h>

#include "timer.h"
#include "texture.h"

typedef uint32_t bool32_t;

// TODO(JRC): This is really ugly and should be fixed is possible.
bool32_t isRunning = true;

int main() {
    static auto processSignal = [] ( int32_t pSignal ) { isRunning = false; };
    signal( SIGINT, processSignal );

    printf( "Start!\n" );

    llce::timer t( 60 );
    while( isRunning ) {
        t.split();

        // TODO(JRC): The criteria for switching should be if a file change
        // is detected on the file handle for the DLL (use 'stat' or equivalent).
        printf( "Frame Rate: %f\r", t.fps() );

        t.wait();
    }

    printf( "\nEnd!\n" );

    return 0;
}
