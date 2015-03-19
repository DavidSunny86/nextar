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
public:

	virtual VisiblePrimitiveList& GetPrimitives(CommitContext& context) = 0;

	/** 0 seconds of expiry time means it will live forever
	 ** @returns Returns the primitive id, can be used to remove it */
	virtual uint32 Register(const AABox3& box, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(Mat4x4R tform, const Color& color = Color::Black,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(PlaneF plane, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(const Frustum& frustum, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(const Geometry& triList, const Color& color,
		float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(const Box2D& rect, const Color& color,
		TextureBase* textured = 0, bool border = true,
		float expiryTimeInSec = 0.0f) = 0;

	virtual void RemovePrimitive(uint32 id) = 0;

	DebugDisplay();
	virtual ~DebugDisplay();
};

} /* namespace nextar */
#endif /* DEBUGINFO_H_ */
