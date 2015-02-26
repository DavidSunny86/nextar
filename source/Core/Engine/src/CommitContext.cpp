/*
 * CommitContext.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <CommitContext.h>

namespace nextar {

CommitContext::CommitContext() :
	frameNumber(0)
	,targetDimension(0, 0)
	,visibiles(nullptr)
	,lightSystem(nullptr)
	,viewport(nullptr)
	,primitive(nullptr)
	,material(nullptr)
	,viewMatrix(nullptr)
	,viewProjectionMatrix(nullptr)
	,projectionMatrix(nullptr)
	,albedoAndGlossMap(nullptr)
	,depthMap(nullptr)
	,normalMap(nullptr)
	,frameTime(0.0f)
	,frameTimer(nullptr) {
	invProjectionMatrix = NEX_NEW(Matrix4x4());
}

CommitContext::~CommitContext() {
	// TODO Auto-generated destructor stub
}

void CommitContext::_Reset() {
	
	frameNumber = 0;
	targetDimension.combined = 0;
	visibiles = nullptr;
	lightSystem = nullptr;
	viewport = nullptr;
	primitive = nullptr;
	material = nullptr;
	light = nullptr;
	albedoAndGlossMap = nullptr;
	normalMap = nullptr;
	depthMap = nullptr;
	frameTime = 0.0f;
	frameTimer = nullptr;
}

} /* namespace nextar */
