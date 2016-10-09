/*
 * DefaultLightSystem.cpp
 *
 *  Created on: 26-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <DefaultLightSystem.h>

namespace nextar {

DefaultLightSystem::DefaultLightSystem() : skyLight(nullptr) {
	// TODO Auto-generated constructor stub

}

DefaultLightSystem::~DefaultLightSystem() {
	// TODO Auto-generated destructor stub
}

void DefaultLightSystem::Prepare() {
	skyLight = nullptr;
	sceneLights.clear();
}

void DefaultLightSystem::PushLight(uint32 sortKey, Light* vl) {
	if (vl->GetLightType() == Light::Type::SKY)
		skyLight = vl;
	else
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

Light* DefaultLightSystem::GetSkyLight() {
	return skyLight;
}

} /* namespace nextar */
