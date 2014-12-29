/*
 * DebugDisplayImpl.h
 *
 *  Created on: 25-Oct-2014
 *      Author: obhi
 */

#ifndef CORE_ENGINE_LOCAL_INCLUDE_DEBUGDISPLAYIMPL_H_
#define CORE_ENGINE_LOCAL_INCLUDE_DEBUGDISPLAYIMPL_H_

#include <DebugDisplay.h>
#include <VisiblePrimitive.h>

namespace nextar {

class DebugPrimitive : public VisiblePrimitive {
protected:
	float timeToDeath;
};

class DebugDisplayImpl: public DebugDisplay {
public:
	DebugDisplayImpl();
	virtual ~DebugDisplayImpl();

	virtual VisiblePrimitiveList& GetPrimitives(CommitContext& context);

	virtual void Register(CommitContext& context, const AABox3& box, const Color& color,
			float expiryTimeInSec = 0.0f);
	virtual void Register(CommitContext& context, Mat3x4R tform, const Color& color = Color::Black,
			float expiryTimeInSec = 0.0f);
	virtual void Register(CommitContext& context, PlaneF plane, const Color& color,
			float expiryTimeInSec = 0.0f);
	virtual void Register(CommitContext& context, const Frustum& frustum, const Color& color,
			float expiryTimeInSec = 0.0f);
	virtual void Register(CommitContext& context, const Box2D& rect, const Color& color,
			TextureBase* textured = 0, bool border = true,
			float expiryTimeInSec = 0.0f);

protected:

	void LoadMaterials();
	void CreateAxisSystemData();

	StreamData axisData;
	VisiblePrimitiveList alivePrimitives;
};

} /* namespace nextar */

#endif /* CORE_ENGINE_LOCAL_INCLUDE_DEBUGDISPLAYIMPL_H_ */
