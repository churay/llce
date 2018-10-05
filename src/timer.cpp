#include <chrono>
#include <ratio>
#include <thread>

#include "timer.h"

namespace llce {

timer::timer( float64_t pRatio, timer::type pType ) {
    LLCE_ASSERT_DEBUG( pRatio > 0.0,
        "Couldn't create timer with invalid fps/spf ratio of " << pRatio << "; "
        "this ratio value must be positive." );

    SecDuration frameDuration( (pType == timer::type::spf) ? pRatio : 1.0 / pRatio );
    mFrameDuration = std::chrono::duration_cast<ClockDuration>( frameDuration );

    ClockPoint initTime = Clock::now();
    mTimerStart = initTime;
    mFrameSplits.fill( initTime );
    mCurrFrameIdx = mPrevFrameIdx = 0;
}


void timer::split( bool32_t pFrameWait ) {
    mPrevFrameIdx = mCurrFrameIdx;
    mCurrFrameIdx = ( mCurrFrameIdx + 1 ) % mFrameSplits.size();
    mFrameSplits[mCurrFrameIdx] = Clock::now();

    if( pFrameWait ) {
        ClockDuration frameTime =
            mFrameSplits[mCurrFrameIdx] - mFrameSplits[mPrevFrameIdx];
        ClockDuration remainingTime = mFrameDuration - frameTime;
        std::this_thread::sleep_for( remainingTime );
    }
}


float64_t timer::ft() const {
    ClockDuration prevFrameTime = std::max(
        mFrameDuration, mFrameSplits[mCurrFrameIdx] - mFrameSplits[mPrevFrameIdx] );
    SecDuration prevFrameSecs = std::chrono::duration_cast<SecDuration>( prevFrameTime );
    return static_cast<float64_t>( prevFrameSecs.count() );
}


float64_t timer::tt() const {
    SecDuration totalTime = std::chrono::duration_cast<SecDuration>( Clock::now() - mTimerStart );
    return static_cast<float64_t>( totalTime.count() );
}


float64_t timer::fps() const {
    return 1.0 / ft();
}


bool32_t timer::cycled() const {
    ClockDuration prevSplitEpoch = mFrameSplits[mPrevFrameIdx] - mTimerStart;
    ClockDuration currSplitEpoch = mFrameSplits[mCurrFrameIdx] - mTimerStart;

    // TODO(JRC): Avoid auto here if possible to avoid type confusion.
    auto prevSplitFrame = prevSplitEpoch / mFrameDuration;
    auto currSplitFrame = currSplitEpoch / mFrameDuration;

    return prevSplitFrame != currSplitFrame;
}

}
