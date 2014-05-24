/*
 * LightSystem.h
 *
 *  Created on: 25-Nov-2013
 *      Author: obhi
 */

#ifndef LIGHTSYSTEM_H_
#define LIGHTSYSTEM_H_

#include <NexBase.h>
#include <VisiblePrimitive.h>
#include <Light.h>
#include <RenderConstants.h>

namespace nextar {

	typedef std::pair<uint32, Light*> KeyVisibleLightPair;
	typedef vector<KeyVisibleLightPair>::type LightList;
	typedef vector< std::reference_wrapper< const KeyVisibleLightPair > >::type SortedLightList;
		
	/* 
	* [     3     |		13	   |	16		]
	*  light type   importance	   depth
	*  todo Will work on it when implementing the light system.
	*/
	class LightKeyHelper {
	public:

		inline bool operator () (const KeyVisibleLightPair& kvp1, const KeyVisibleLightPair& kvp2) {
			return kvp1.first < kvp2.first;
		}
	};
	/**
	 * @remarks Scene traversal will involve lights being pushed into
	 * light system, which will later be used by the render system to
	 * determine scene lighting for the current frame.
	 */
	class _NexEngineAPI LightSystem : public Referenced<LightSystem, AllocScene> {
	public:
		enum {
			/* The first 255 sorted lights are taken into account for forward lights */
			FORWARD_MAX_LIGHT_COUNT = (uint32)RenderConstants::FORWARD_MAX_LIGHT_COUNT,
			FORWARD_MAX_LIGHT_PER_PRIMITIVE = (uint32)RenderConstants::FORWARD_MAX_LIGHT_PER_PRIMITIVE,
		};

		typedef array<uint8, FORWARD_MAX_LIGHT_PER_PRIMITIVE>::type LightSet;

		LightSystem();
		virtual ~LightSystem();

		virtual void Prepare() = 0;
		virtual void PushLight(uint32 sortKey, Light*) = 0;
		virtual void Sort() = 0;
		virtual void NearbyLights(LightSet& ls, BoundingVolume*) = 0;
		virtual LightList& GetLights() = 0;
	};

} /* namespace nextar */
#endif /* LIGHTSYSTEM_H_ */
