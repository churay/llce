#ifndef LLCE_TIMER_H
#define LLCE_TIMER_H

#include <chrono>
#include <ratio>
#include "consts.h"

namespace llce {

class timer {
    public:

    /// Constructors ///

    timer( uint32_t pFPS = 60 );

    /// Class Functions ///

    void split();
    void wait();

    float64_t fps( uint32_t pNumFrames = 1 ) const;
    float64_t dt( uint32_t pNumFrames = 1 ) const;

    private:

    /// Class Setup ///

    using Clock = std::chrono::high_resolution_clock;
    using ClockPoint = decltype( Clock::now() );
    using ClockDuration = decltype( Clock::now() - Clock::now() );
    using SecDuration = std::chrono::duration<float64_t, std::ratio<1>>;

    /// Class Fields ///

    ClockDuration mFrameDuration;
    ClockPoint mSplitTime, mWaitTime;
};

}

#endif
