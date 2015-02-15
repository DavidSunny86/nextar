/*
 * DebugInfo.h
 *
 *  Created on: 04-Jan-2014
 *      Author: obhi
 */

#ifndef DEBUGINFO_H_
#define DEBUGINFO_H_

#include <StreamData.h>
#include <MathBase.h>
#include <Color.h>
#include <Frustum.h>
#include <VisiblePrimitive.h>

namespace nextar {

class CommitContext;

class DebugDisplay: public AllocGraphics {
public:

	virtual VisiblePrimitiveList& GetPrimitives(CommitContext& context) = 0;

	/** 0 seconds of expiry time means it will live for ever
	 ** @returns Returns the primitive id, can be used to remove it */
	virtual uint32 Register(CommitContext& context, const AABox3& box, const Color& color,
			float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(CommitContext& context, Mat3x4R tform, const Color& color = Color::Black,
			float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(CommitContext& context, PlaneF plane, const Color& color,
			float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(CommitContext& context, const Frustum& frustum, const Color& color,
			float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(CommitContext& context, const Polygon& triList, const Color& color,
				float expiryTimeInSec = 0.0f) = 0;
	virtual uint32 Register(CommitContext& context, const Box2D& rect, const Color& color,
			TextureBase* textured = 0, bool border = true,
			float expiryTimeInSec = 0.0f) = 0;

	virtual void RemovePrimitive(uint32 id) = 0;

	DebugDisplay();
	virtual ~DebugDisplay();
};

} /* namespace nextar */
#endif /* DEBUGINFO_H_ */
