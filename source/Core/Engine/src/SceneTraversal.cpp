/*
 * SceneTraversal.cpp
 *
 *  Created on: 18-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <SceneTraversal.h>
#include <Camera.h>

namespace nextar {

SceneTraversal::SceneTraversal(Camera* cam, VisibilitySet* visList,
		BoundingVolume* vol) :
		camera(cam)
		,visibilitySet(visList)
		,visibleBoundsInfo(vol)
		,distanceInView(65000)
		,recipDistanceInView(1 / 65000.f)
		,frameTimer(nullptr)
		,moveable(nullptr)
		,frustum(nullptr)
		,lightSystem(nullptr)
		,scene(nullptr)
		,visibilityMask(VISIBILITY_ALL) 	{
	if (camera)
		visibilityMask = camera->GetVisibilityMask();
}

SceneTraversal::~SceneTraversal() {
}

} /* namespace nextar */
