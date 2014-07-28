/*
 * CullingSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef BVCULLINGSYSTEM_H_
#define BVCULLINGSYSTEM_H_

#include <NexBase.h>
#include <CullingSystem.h>

namespace nextar {

/** 
 * Culling systems derive from spatial to support hybrid culler hierarchies.
 */
class BVCullingSystem: public CullingSystem {
public:
	typedef vector<Spatial*>::type SpatialList;

	BVCullingSystem(const StringID name,
			const StringID factory,
			Component* parent = nullptr);
	virtual ~BVCullingSystem();

	virtual void AddBody(Spatial*) override;
	virtual void RemoveBody(Spatial*) override;
	virtual void Visit(SceneTraversal& trav) override;

protected:
	Camera* coherentCam;
	SpatialList bodies;
};

} /* namespace nextar */

#endif /* BVCULLINGSYSTEM_H_ */
