/*
 * SceneTraversal.cpp
 *
 *  Created on: 18-Nov-2013
 *      Author: obhi
 */
#include "NexEngine.h"
#include "SceneTraversal.h"

namespace nextar {

	SceneTraversal::SceneTraversal(Camera* cam, VisibilitySet* visList, BoundingVolume* vol) :
				camera(cam), visibilitySet(visList), visibleBoundsInfo(vol),
				distanceInView(65000), recipDistanceInView(1/65000.f), frameNumber(0) {
		if(camera)
			visibilityMask = camera->GetVisibilityMask();
	}

	SceneTraversal::~SceneTraversal() {
		// TODO Auto-generated destructor stub
	}

} /* namespace nextar */
