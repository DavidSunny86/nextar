/*
 * SceneTraversal.h
 *
 *  Created on: 18-Nov-2013
 *      Author: obhi
 */

#ifndef SCENETRAVERSAL_H_
#define SCENETRAVERSAL_H_

#include <NexBase.h>

namespace nextar {

enum VisibilityMask : uint32 {
	VISIBILITY_BACKGROUND = 1 << 0,
	VISIBILITY_OPAQUE = 1 << 1,
	VISIBILITY_TRANSLUCENT = 1 << 3,
	VISIBILITY_OVERLAY = 1 << 4,
	VISIBILITY_SHADOW_CASTER = 1 << 5,
	VISIBILITY_SHADOW_RECEIVER = 1 << 6,
	VISIBILITY_ALL = 0xffffffff
};

NEX_ENUM_FLAGS(VisibilityMask, uint32);

class SceneTraversal {
public:
	SceneTraversal(Camera* camera = nullptr, VisibilitySet* visList = nullptr,
			BoundingVolume* vol = nullptr);
	virtual ~SceneTraversal();

	Moveable* moveable;
	Camera* camera;
	Frustum* frustum;
	VisibilitySet* visibilitySet;
	BoundingVolume* visibleBoundsInfo;
	LightSystem* lightSystem;
	SceneAsset* scene;
	/* Layer mask */
	VisibilityMask visibilityMask;
	uint32 frameNumber;
	/** scene parameter */
	float distanceInView;
	float recipDistanceInView;
};
} /* namespace nextar */
#endif /* SCENETRAVERSAL_H_ */
