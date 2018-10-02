#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "keyboard.h"

namespace llce {

keyboard::keyboard() {
    // NOTE(JRC): This command makes STDIN a non-blocking input source.
    fcntl( STDIN_FILENO, F_SETFL, (fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK) );

    // NOTE(JRC): This code is a modified version of the code on GCat's
    // technical blog on raw keyboard input from Linux that doesn't add raw
    // support due to bad driver support for 'ioctl' (see: http://www.gcat.org.uk/tech/?p=70).
    mOldSettings = {};
    if( !tcgetattr(STDIN_FILENO, &mOldSettings) ) {
        struct termios newTerm = mOldSettings;
        newTerm.c_lflag &= -( ICANON | ECHO | ISIG );
        newTerm.c_iflag &= -( ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF );
        // TODO(JRC): Verify that the 'tcsetattr' call fully worked before
        // allowing this 'keyboard' instance to register that it's running.
        mReading = !tcsetattr( STDIN_FILENO, TCSANOW, &newTerm );

        // TODO(JRC): The following lines are supposed to make the terminal
        // a raw input terminal, but this setting ends up adding more complications
        // than is worthwhile for this example.
        // cfmakeraw( &newTerm );
        // mReading = !tcsetattr( STDIN_FILENO, TCSANOW, &newTerm );
    }
}


keyboard::~keyboard() {
    if( reading() ) {
        tcsetattr( STDIN_FILENO, TCSAFLUSH, &mOldSettings );
    }
}


void keyboard::read( bool8_t* pBuffer ) const {
    memset( pBuffer, false, sizeof(bool8_t) * keyboard::keycode::length );

    if( reading() ) {
        char8_t nextKey;
        while( fread(&nextKey, sizeof(nextKey), 1, stdin) > 0 ) {
            if( nextKey == 0x71 ) {
                pBuffer[keyboard::keycode::q] = true;
            } else if( nextKey == 0x72 ) {
                pBuffer[keyboard::keycode::r] = true;
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
    return mReading;
}

}
