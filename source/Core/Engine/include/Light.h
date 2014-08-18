/*
 * Light.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include <Renderable.h>

namespace nextar {

class Light: public Renderable {
public:

	enum {
		CLASS_ID = Component::CLASS_LIGHT, CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	enum class Type	: uint16 {
		SKY, DIRECTIONAL, OMNI, SPOT, AREA, TYPE_COUNT,
	};

	enum class PassIndex : uint16 {
		SKY = 100,
		DIRECTIONAL = 1,
		OMNI = 0,
		SPOT = 2,
		AREA = 100,
	};

	typedef ComponentTraits<Light> Traits;

	Light(const StringID name, const StringID factory, Component* parent = nullptr);
	virtual ~Light();

	inline VisiblePrimitive* GetLightVolume() {
		return &lightVolume;
	}

	inline Type GetLightType() const {
		return lightType;
	}

	inline void SetLightColor(const Color& c) {
		diffuseColor = c;
	}

	inline const Color& GetLightColor() const {
		return diffuseColor;
	}

	virtual void SetMoveable(Moveable* ptr);
	void SetLightRange(float range);

	/** @brief Get node type */
	virtual uint32 GetClassID() const;

	virtual void Visit(SceneTraversal& traversal);
	virtual void SetMaterial(uint32 index, MaterialAssetPtr& mtl);

protected:
	Type lightType;
	uint32 sortKey;
	Color diffuseColor;
	Color specularColor;
	/* Fall off angle for spot lights */
	float falloffAngle;
	float lightRange;

	VisiblePrimitive lightVolume;
};

} /* namespace nextar */
#endif /* LIGHT_H_ */
