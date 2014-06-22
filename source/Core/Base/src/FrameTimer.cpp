/* 
 * File:   FrameTimer.cpp
 * Author: obhi
 * 
 * Created on June 16, 2011, 8:49 AM
 */
#include <BaseHeaders.h>
#include <FrameTimer.h>

namespace nextar {

FrameTimer::FrameTimer() :
		frameNumber(0), timeFactor(1.0f), elapsedTime(0), frameTime(0) {
}

FrameTimer::~FrameTimer() {
}

}
