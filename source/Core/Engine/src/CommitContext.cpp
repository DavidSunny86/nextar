/*
 * CommitContext.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <CommitContext.h>

namespace nextar {

CommitContext::CommitContext() {
	_Reset();
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
	viewMatrix = nullptr;
	viewProjectionMatrix = nullptr;
	projectionMatrix = nullptr;
	invProjectionMatrix = nullptr;
	invViewProjectionMatrix = nullptr;
	light = nullptr;
	albedoMap = nullptr;
	specularMap = nullptr;
	normalMap = nullptr;
	depthMap = nullptr;
	frameTime = 0.0f;
	frameTimer = nullptr;

	for (auto &b : paramBuffers)
		b = nullptr;

	sunLightIntensity.x = 0.4f;
	sunLightIntensity.y = 0.4f;
	sunLightIntensity.z = 0.4f;
	sunLightIntensity.w = 1.0f;
	sunLightPosition.x = 250;
	sunLightPosition.y = 1000;
	sunLightPosition.z = 250;
	sunLightPosition.w = 0;
	sunLightColor = Color::White;
}

} /* namespace nextar */
