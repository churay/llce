#include <chrono>
#include <ratio>
#include <thread>

#include "timer.h"

timer::timer( size_t pFPS ) {
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

    auto elapsedTime = mWaitTime - mSplitTime;
    auto remainingTime = mFrameDuration - elapsedTime;
    std::this_thread::sleep_for( remainingTime );
}


double timer::fps( size_t pNumFrames ) const {
    // TODO(JRC): Implement this function so that it properly outputs the average
    // FPS for the past "pNumFrames" frames.
    SecDuration prevFrameTime = std::chrono::duration_cast<SecDuration>( mWaitTime - mSplitTime );
    return static_cast<double>( 1.0 / prevFrameTime.count() );
}


double timer::dt( size_t pNumFrames ) const {
    // TODO(JRC): Implement this function so that it properly outputs the average
    // time delta for the past "pNumFrames" frames.
    SecDuration prevFrameTime = std::chrono::duration_cast<SecDuration>( mWaitTime - mSplitTime );
    return static_cast<double>( prevFrameTime.count() );
}
