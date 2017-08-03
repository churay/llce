#include <chrono>
#include <ratio>
#include <thread>

#include "timer.h"

namespace llce {

timer::timer( uint32_t pFPS ) {
    SecDuration frameDuration( 1.0 / pFPS );

    mFrameDuration = std::chrono::duration_cast<ClockDuration>( frameDuration );
    mSplitTime = Clock::now();
    mWaitTime = Clock::now();
}


void timer::split() {
    mSplitTime = Clock::now();
}


void timer::wait() {
    mWaitTime = Clock::now();

    ClockDuration elapsedTime = mWaitTime - mSplitTime;
    ClockDuration remainingTime = mFrameDuration - elapsedTime;
    std::this_thread::sleep_for( remainingTime );
}


float64_t timer::fps( uint32_t pNumFrames ) const {
    // TODO(JRC): Implement this function so that it properly outputs the average
    // FPS for the past "pNumFrames" frames.
    SecDuration prevFrameTime = std::chrono::duration_cast<SecDuration>( mWaitTime - mSplitTime );
    return static_cast<float64_t>( 1.0 / prevFrameTime.count() );
}


float64_t timer::dt( uint32_t pNumFrames ) const {
    // TODO(JRC): Implement this function so that it properly outputs the average
    // time delta for the past "pNumFrames" frames.
    SecDuration prevFrameTime = std::chrono::duration_cast<SecDuration>( mWaitTime - mSplitTime );
    return static_cast<float64_t>( prevFrameTime.count() );
}

}
