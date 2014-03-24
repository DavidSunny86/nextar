/*
 * VisiblePrimitive.h
 *
 *  Created on: Jul 25, 2011
 *      Author: obhi
 */

#ifndef VISIBLEPRIMITIVE_H_
#define VISIBLEPRIMITIVE_H_

#include <NexBase.h>
#include <StreamData.h>
#include <Color.h>
#include <MathBase.h>
#include <MaterialAsset.h>

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

		void SetWorldMatrices(const Matrix4x4* matrices, int16 numMatrices) {
			worldMatrices = matrices;
			numWorldMatrices = numMatrices;
		}

		void SetBoundsInfo(const BoundingVolume* bounds) {
			this->bounds = bounds;
		}

		void SetMaterial(MaterialAssetPtr& m) {
			material = m;
		}

	protected:
		/* Shader rendering flags */
		uint16 renderFlags;
		/* Number of world matrices */
		int16 numWorldMatrices;
		/* bounding volume */
		const BoundingVolume* bounds;
		/* The current world matrices */
		const Matrix4x4* worldMatrices;
		/* Material */
		MaterialAssetPtr material;
		/** stream data to render */
		StreamData streamData;
	};

} /* namespace nextar */
#endif /* VISIBLEPRIMITIVE_H_ */
