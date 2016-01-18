/*
 * DebugInfo.h
 *
 *  Created on: 04-Jan-2014
 *      Author: obhi
 */

#ifndef DEBUGINFO_H_
#define DEBUGINFO_H_

#include <NexEngine.h>
#include <StreamData.h>
#include <MathBase.h>
#include <Color.h>
#include <Frustum.h>
#include <VisiblePrimitive.h>
#include <Geometry.h>

namespace nextar {

class CommitContext;

class _NexEngineAPI DebugDisplay: public AllocGraphics,
	public Singleton<DebugDisplay> {
	
protected:
	virtual VisiblePrimitiveList& _GetPrimitives(CommitContext& context) = 0;
	/** 0 seconds of expiry time means it will live forever
	  * negetive value for expiry time means lifetime is 1 frame
	 ** @returns Returns the primitive id, can be used to remove it */
	virtual uint32 _Register(AABoxF box, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 _Register(Mat4x4R tform, float screenSpaceFactor = 0.8f, const Color& color = Color::White,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 _Register(PlaneF plane, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 _Register(const Frustum& frustum, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 _Register(const Geometry& triList, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 _Register(const Box2D& rect, const Color& color,
		Vec4AF textureOffsetAndRepeat,
		TextureBase* textured = 0, bool border = true,
		float expiryTimeInSec = 0.0f) = 0;

	virtual void _RemovePrimitive(uint32 id) = 0;

	DebugDisplay();
	virtual ~DebugDisplay();

public:

	static inline VisiblePrimitiveList& GetPrimitives(CommitContext& context) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._GetPrimitives(context);
		else {
			static VisiblePrimitiveList dummy;
			return dummy;
		}
	}
	/** 0 seconds of expiry time means it will live forever
	* negetive value for expiry time means lifetime is 1 frame
	** @returns Returns the primitive id, can be used to remove it */
	static inline uint32 Register(AABoxF box, const Color& color,
		float expiryTimeInSec = 0.0f) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._Register(box, color, expiryTimeInSec);
		return -1;
	}

	static inline uint32 Register(Mat4x4R tform, float screenSpaceFactor = 0.8f,
		const Color& color = Color::White,
		float expiryTimeInSec = 0.0f) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._Register(tform, screenSpaceFactor, color, expiryTimeInSec);
		return -1;
	}

	static inline uint32 Register(PlaneF plane, const Color& color,
		float expiryTimeInSec = 0.0f) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._Register(plane, color, expiryTimeInSec);
		return -1;
	}

	static inline uint32 Register(const Frustum& frustum, const Color& color,
		float expiryTimeInSec = 0.0f) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._Register(frustum, color, expiryTimeInSec);
		return -1;
	}

	static inline uint32 Register(const Geometry& triList, const Color& color,
		float expiryTimeInSec = 0.0f) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._Register(triList, color, expiryTimeInSec);
		return -1;
	}

	static inline uint32 Register(const Box2D& rect, const Color& color,
		Vec4AF textureOffsetAndRepeat,
		TextureBase* textured = 0, bool border = true,
		float expiryTimeInSec = 0.0f) {
		if (DebugDisplay::InstancePtr())
			return DebugDisplay::Instance()._Register(rect, color, textureOffsetAndRepeat, textured, border, expiryTimeInSec);
		return -1;
	}

	static inline void RemovePrimitive(uint32 id) {
		if (DebugDisplay::InstancePtr())
			DebugDisplay::Instance()._RemovePrimitive(id);
	}


};

} /* namespace nextar */
#endif /* DEBUGINFO_H_ */
