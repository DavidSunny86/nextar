/*
 * VisiblePrimitive.h
 *
 *  Created on: Jul 25, 2011
 *      Author: obhi
 */

#ifndef VISIBLEPRIMITIVE_H_
#define VISIBLEPRIMITIVE_H_

#include "NexSys.h"
#include "StreamData.h"
#include "Color.h"
#include "NexMath.h"

namespace nextar {

	class VisiblePrimitive {
	public:

		StreamData* GetStreamData() {
			return &streamData;
		}

		MaterialAsset* GetMaterial() {
			return material;
		}

		int16 GetNumWorldMatrices() const {
			return numWorldMatrices;
		}

		const Matrix4x4* GetWorldMatrices() const {
			return worldMatrices;
		}

	protected:
		/* Shader rendering flags */
		uint16 renderFlags;
		/* Number of world matrices */
		int16 numWorldMatrices;
		/* bounding volume */
		BoundingVolume* bounds;
		/* The current world matrices */
		Matrix4x4* worldMatrices;
		/* Material */
		MaterialAsset* material;
		/** stream data to render */
		StreamData streamData;
	};

} /* namespace nextar */
#endif /* VISIBLEPRIMITIVE_H_ */
