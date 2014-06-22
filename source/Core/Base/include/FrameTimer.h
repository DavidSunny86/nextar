/* 
 * File:   FrameTimer.h
 * Author: obhi
 *
 * Created on June 16, 2011, 8:49 AM
 */

#ifndef FRAMETIMER_H
#define	FRAMETIMER_H

#include <NexBase.h>
#include <FrameListener.h>

namespace nextar {

/* All time units are seconds */
class FrameTimer {
public:
	FrameTimer();
	~FrameTimer();

	/* Called to mark the start of a frame, return false to skip frame */
	inline void BeginFrame() {
		this->frameNumber++;
	}

	inline void EndFrame(uint32 timeElapsed) {
		frameTime = ((float) timeElapsed * .001f) * timeFactor;
		elapsedTime += frameTime;
	}

	void SetTimeFactor(float timeFactor) {
		this->timeFactor = timeFactor;
	}

	float GetTimeFactor() const {
		return timeFactor;
	}

	void SetFrameTime(float frameTime) {
		this->frameTime = frameTime;
	}

	float GetFrameTime() const {
		return frameTime;
	}

	void SetElapsedTime(float elapsedTime) {
		this->elapsedTime = elapsedTime;
	}

	float GetElapsedTime() const {
		return elapsedTime;
	}

	uint32 GetFrameNumber() const {
		return frameNumber;
	}

private:

	float elapsedTime;
	float frameTime;
	float timeFactor;
	uint32 frameNumber;
};
}

#endif	/* FRAMETIMER_H */

