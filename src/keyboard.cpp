#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <linux/kd.h>
#include <sys/ioctl.h>

#include "keyboard.h"

namespace llce {

keyboard::keyboard() {
    // NOTE(JRC): This command makes STDIN a non-blocking input source.
    fcntl( STDIN_FILENO, F_SETFL, (fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK) );

    mOverwrittenTerm = {};
    mOverwrittenMode = -1;

    if( ioctl(STDIN_FILENO, KDGKBMODE, &mOverwrittenMode) >= 0 ) {
        tcgetattr( STDIN_FILENO, &mOverwrittenTerm );

        // NOTE(JRC): These commands disable buffering, echoing, and key processing
        // in the TTY for the process (see: http://www.gcat.org.uk/tech/?p=70).
        struct termios newTerm = mOverwrittenTerm;
        newTerm.c_lflag &= -( ICANON | ECHO | ISIG );
        newTerm.c_iflag &= -( ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF );
        tcsetattr( STDIN_FILENO, TCSANOW, &newTerm );

        ioctl( STDIN_FILENO, KDSKBMODE, K_RAW );
    }
}


keyboard::~keyboard() {
    if( reading() ) {
        tcsetattr( STDIN_FILENO, TCSAFLUSH, &mOverwrittenTerm );
        ioctl( STDIN_FILENO, KDSKBMODE, mOverwrittenMode );
    }
}


void keyboard::read( bool8_t* pBuffer ) const {
    memset( pBuffer, false, sizeof(bool8_t) * keyboard::keycode::length );

    if( reading() ) {
        char nextKey;
        while( fread(&nextKey, sizeof(nextKey), 1, stdin) > 0 ) {
            if( nextKey == 0x71 ) {
                pBuffer[keyboard::keycode::q] = true;
            } else if( nextKey == 0x68 ) {
                pBuffer[keyboard::keycode::h] = true;
            } else if( nextKey == 0x6A ) {
                pBuffer[keyboard::keycode::j] = true;
            } else if( nextKey == 0x6B ) {
                pBuffer[keyboard::keycode::k] = true;
            } else if( nextKey == 0x6C ) {
                pBuffer[keyboard::keycode::l] = true;
            }
        }
    }
}


bool32_t keyboard::reading() const {
    return mOverwrittenMode >= 0;
}

}
