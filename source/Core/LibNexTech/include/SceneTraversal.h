/*
 * SceneTraversal.h
 *
 *  Created on: 18-Nov-2013
 *      Author: obhi
 */

#ifndef SCENETRAVERSAL_H_
#define SCENETRAVERSAL_H_

#include "NexTypes.h"

namespace nextar {

	class SceneTraversal {
	public:
		SceneTraversal(Camera* cam, VisibilitySet* visList, BoundingVolume* vol);
		SceneTraversal(VisibilitySet* visList = 0, BoundingVolume* vol = 0);
		virtual ~SceneTraversal();


		Moveable* currentMoveable;

		Camera* camera;
		VisibilitySet* visibilitySet;
		BoundingVolume* visibleBoundsInfo;
		LightSystem* lightSystem;
		/* Layer mask */
		uint32 visibilityMask;
		uint32 frameNumber;
		/** scene parameter */
		float distanceInView;
		float recipDistanceInView;
	};
} /* namespace nextar */
#endif /* SCENETRAVERSAL_H_ */
