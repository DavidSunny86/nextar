/*
 * Light.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "Renderable.h"

namespace nextar {

	class Light: public nextar::Component {
	public:

		enum {
			TYPE = TYPE_LIGHT,
			CATAGORY = CAT_LIGHT,
		};

		enum class Type : uint16 {
			DIRECTIONAL,
			OMNI,
			SPOT,
			SUN,
			AREA,
			TYPE_COUNT,
		};

		Light(ComponentManager* creator, const String& name);
		virtual ~Light();

		/** @brief Get node type */
		virtual int GetComponentType() const;
		virtual int GetComponentCatagory() const;

		virtual void RegisterVisibles(SceneTraversal& traversal);

	protected:
		Color diffuseColor;
		Color specularColor;
		Color ambientColor;
		/* Fall off angle for spot lights */
		float falloffAngle;

		union {
			struct {
				float attenRange;
				float attenConstant;
				float attenLinear;
				float attenQuadratic;
			};

			float attenuation[4];
		};

		MaterialAssetPtr lightMaterial;
	};

} /* namespace nextar */
#endif /* LIGHT_H_ */
