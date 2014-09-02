/*
 * DefaultLightSystem.cpp
 *
 *  Created on: 26-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <DefaultLightSystem.h>

namespace nextar {

DefaultLightSystem::DefaultLightSystem() {
	// TODO Auto-generated constructor stub

}

DefaultLightSystem::~DefaultLightSystem() {
	// TODO Auto-generated destructor stub
}

void DefaultLightSystem::Prepare() {
	sceneLights.clear();
}

void DefaultLightSystem::PushLight(uint32 sortKey, Light* vl) {
	sceneLights.emplace_back(sortKey, vl);
}

LightList& DefaultLightSystem::GetLights() {
	return sceneLights;
}

void DefaultLightSystem::NearbyLights(LightSet& ls, BoundingVolume*) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

void DefaultLightSystem::Sort() {
	std::sort(sceneLights.begin(), sceneLights.end(), LightKeyHelper());
}
} /* namespace nextar */