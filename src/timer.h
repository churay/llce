#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <ratio>

class timer {
    public:

    /// Constructors ///

    timer( size_t pFPS = 60 );

    /// Class Functions ///

    void split();
    void wait();

    double fps( size_t pNumFrames = 1 ) const;
    double dt( size_t pNumFrames = 1 ) const;

    private:

    /// Class Setup ///

    using Clock = std::chrono::high_resolution_clock;
    using ClockPoint = decltype( Clock::now() );
    using ClockDuration = decltype( Clock::now() - Clock::now() );
    using SecDuration = std::chrono::duration<long double, std::ratio<1>>;

    /// Class Fields ///

    ClockDuration mFrameDuration;
    ClockPoint mSplitTime, mWaitTime;
};

#endif
