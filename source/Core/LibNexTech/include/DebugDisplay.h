/*
 * DebugInfo.h
 *
 *  Created on: 04-Jan-2014
 *      Author: obhi
 */

#ifndef DEBUGINFO_H_
#define DEBUGINFO_H_

#include "StreamData.h"
#include "MathBase.h"
#include "Color.h"
#include "Frustum.h"

namespace nextar {

	class DebugDisplay : public AllocGraphics {
	public:

		virtual void Register(const AABox3& box, const Color& color, float expiryTimeInSec = 0.0f) = 0;
		virtual void Register(Mat3x4R tform, const Color& color, float expiryTimeInSec = 0.0f) = 0;
		virtual void Register(PlaneF plane, const Color& color, float expiryTimeInSec = 0.0f) = 0;
		virtual void Register(const Frustum& frustum, const Color& color, float expiryTimeInSec = 0.0f) = 0;
		virtual void Register(const Box2D& rect, const Color& color, TextureBase* textured = 0, bool border = true, float expiryTimeInSec = 0.0f) = 0;

		DebugDisplay();
		virtual ~DebugDisplay();
	};

} /* namespace nextar */
#endif /* DEBUGINFO_H_ */
