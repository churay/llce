#ifndef LLCE_KEYBOARD_H
#define LLCE_KEYBOARD_H

#include "termios.h"
#include "consts.h"

namespace llce {

class keyboard {
    public:

    /// Class Attributes ///

    enum keycode { q, h, j, k, l, length };

    /// Constructors ///

    keyboard();
    ~keyboard();

    /// Class Functions ///

    void read( bool8_t* pBuffer ) const;
    bool32_t reading() const;

    private:

    /// Class Fields ///

    struct termios mOverwrittenTerm;
    int32_t mOverwrittenMode;
};

}

#endif
