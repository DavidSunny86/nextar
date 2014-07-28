/*
 * CullingSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef CULLINGSYSTEM_H_
#define CULLINGSYSTEM_H_

#include <NexBase.h>
#include <Spatial.h>

namespace nextar {

/** 
 * Culling systems derive from spatial to support hybrid culler hierarchies.
 */
class CullingSystem: public Spatial {
public:
	CullingSystem(const StringID name, const StringID factory,  Component* parent =
			nullptr);
	virtual ~CullingSystem();

	virtual void AddBody(Spatial*) = 0;
	virtual void RemoveBody(Spatial*) = 0;

	virtual void Visit(SceneTraversal& trav) override = 0;
};

} /* namespace nextar */
#endif /* CULLINGSYSTEM_H_ */
