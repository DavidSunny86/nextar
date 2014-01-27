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

	class RenderableLight: public nextar::Renderable {
	public:

		enum {
			TYPE = COMPONENT_RENDERABLE_LIGHT
		};

		enum class Type : uint16 {
			DIRECTIONAL,
			OMNI,
			SPOT,
			SUN,
			AREA,
			TYPE_COUNT,
		};

		RenderableLight();
		virtual ~RenderableLight();

		/** @brief Get node type */
		virtual int GetType() const;

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

		MaterialPtr lightMaterial;
	};

} /* namespace nextar */
#endif /* LIGHT_H_ */
