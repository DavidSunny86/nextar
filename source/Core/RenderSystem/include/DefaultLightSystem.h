/*
 * DefaultLightSystem.h
 *
 *  Created on: 26-Nov-2013
 *      Author: obhi
 */

#ifndef DEFAULTLIGHTSYSTEM_H_
#define DEFAULTLIGHTSYSTEM_H_

#include <LightSystem.h>

namespace nextar {

	class DefaultLightSystem: public nextar::LightSystem {
	public:
		DefaultLightSystem();
		virtual ~DefaultLightSystem();

		virtual void Prepare();
		virtual void PushLight(uint32 sortKey, VisibleLight*);
		virtual void Sort();
		virtual void NearbyLights(LightSet& ls, BoundingVolume*);
		virtual LightList& GetLights();

	protected:

		LightList sceneLights;
	};

} /* namespace nextar */
#endif /* DEFAULTLIGHTSYSTEM_H_ */
