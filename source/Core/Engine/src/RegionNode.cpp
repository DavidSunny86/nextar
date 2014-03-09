/* 
 * File:   Region.cpp
 * Author: obhi
 * 
 * Created on February 19, 2011, 9:06 AM
 */
#include "NexEngine.h"
#include "NexSys.h"
#include "RenderContext.h"
#include "Shader.h"
#include "LightCamera.h"
#include "scene/RegionNode.h"

namespace nextar {

	RegionNode::RegionNode(ComponentManager *creator,
			const String& name, bool allocMatrixBuff)
			: ContainerNode(creator, name, allocMatrixBuff), cullingSystem(0) {
		flags |= NEEDS_CHILD_NOTIFICATION;
		regionNode = this;
	}

	RegionNode::~RegionNode() {
	}

	void RegionNode::NotifyNodeAdded(Movable* node) {
		if (cullingSystem)
			cullingSystem->NotifyNodeAdded(node);
		node->SetRegion(this);
	}

	void RegionNode::NotifyNodeRemoved(Movable* node) {
		if (cullingSystem)
			cullingSystem->NotifyNodeRemoved(node);
		node->SetRegion(0);
	}
}

