/*
 * CullingSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef BVCULLINGSYSTEM_H_
#define BVCULLINGSYSTEM_H_

#include "NexSys.h"
#include "CullingSystem.h"

namespace nextar {

	/** 
	 * Culling systems derive from spatial to support hybrid culler hierarchies.
	 */
	class BVCullingSystem : public CullingSystem {
	public:
		typedef vector<Spatial*>::type SpatialList;

		BVCullingSystem(ComponentManager *creator = nullptr, const String& name = StringUtils::Null, Component* parent = nullptr);
		virtual ~BVCullingSystem();
				
		virtual void AddBody(Spatial*);
		virtual void RemoveBody(Spatial*);

		virtual void Visit(SceneTraversal& trav);

	protected:
		Camera* coherentCam;
		SpatialList bodies;
	};

} /* namespace nextar */

#endif /* BVCULLINGSYSTEM_H_ */
