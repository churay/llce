#ifndef LLCE_TIMER_H
#define LLCE_TIMER_H

#include <array>
#include <chrono>
#include <ratio>
#include "consts.h"

namespace llce {

class timer {
    public:

    /// Class Attributes ///

    enum class type { fps, spf };

    const static uint32_t CACHE_SIZE = 10;

    /// Constructors ///

    timer( float64_t pRatio = 60.0, type pType = type::fps );

    /// Class Functions ///

    void split( bool32_t pWaitFrame = false );

    float64_t ft() const;
    float64_t tt() const;
    float64_t fps() const;

    bool32_t cycled() const;

    private:

    /// Class Setup ///

    using Clock = std::chrono::high_resolution_clock;
    using ClockPoint = decltype( Clock::now() );
    using ClockDuration = decltype( Clock::now() - Clock::now() );
    using SecDuration = std::chrono::duration<float64_t, std::ratio<1>>;

    /// Class Fields ///

    ClockDuration mFrameDuration;
    ClockPoint mTimerStart;

    std::array<ClockPoint, CACHE_SIZE> mFrameSplits;
    uint32_t mCurrFrameIdx, mPrevFrameIdx;
};

}

#endif
