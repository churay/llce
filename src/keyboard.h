#ifndef LLCE_KEYBOARD_H
#define LLCE_KEYBOARD_H

#include "termios.h"
#include "consts.h"

namespace llce {

class keyboard {
    public:

    /// Class Attributes ///

    enum keycode { q, r, h, j, k, l, length };

    /// Constructors ///

    keyboard();
    ~keyboard();

    /// Class Functions ///

    void read( bool8_t* pBuffer ) const;
    bool32_t reading() const;

    private:

    /// Class Fields ///

    bool32_t mReading;
    struct termios mOldSettings;
};

}

#endif
