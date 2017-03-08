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
#include <NexMath.h>
#include <MaterialAsset.h>

namespace nextar {



class _NexEngineAPI VisiblePrimitive : public AllocGeneral {
public:

	struct RenderInformation {};

	VisiblePrimitive() :
		renderFlags(0)
		,numWorldMatrices(0)
		,bounds(nullptr)
		,worldMatrices(nullptr)
		,streamData(nullptr)
		,parameters(nullptr)
		,_reserved(nullptr){
	}

	virtual ~VisiblePrimitive() {}

	StreamData* GetStreamData() {
		return streamData;
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

	void SetStreamData(StreamData* stream) {
		streamData = stream;
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

	void SetParameters(ParameterBuffer* p) {
		parameters = p;
	}

	const ParameterBuffer* GetParameters() const {
		return parameters;
	}

	ParameterBuffer* GetParameters() {
		return parameters;
	}
	// Overrides
	virtual float GetConstantSize() const {
		return 0.0f;
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
	StreamData* streamData;
	/* Draw information */
	DrawInformation* _reserved;
	/* object parameters */
	ParameterBuffer* parameters;
};

typedef vector<VisiblePrimitive*>::type VisiblePrimitiveList;

} /* namespace nextar */
#endif /* VISIBLEPRIMITIVE_H_ */
